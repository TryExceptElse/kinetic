/**
    Copyright 2018 TryExceptElse

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

#include <iterator>
#include "path.h"

namespace kin {


static constexpr double kMaxMassRatioChangePerStep = 0.001;

// Maneuver methods ---------------------------------------------------





// FlightPath methods -------------------------------------------------


FlightPath::FlightPath(
    const System &system, const Vector r, const Vector v, double t):
        system_(system), r0_(r), v0_(v), t0_(t) {}

Orbit FlightPath::Predict(const double time) const {
    return GetSegment(time).Predict(time);
}

const Maneuver* FlightPath::FindManeuver(const double t) const {
    if (maneuvers_.size() == 0) {
        return nullptr;
    }
    auto following_iterator = maneuvers_.upper_bound(t);
    // If no previous iterator exists, return nullptr
    if (following_iterator == maneuvers_.begin()) {
        return nullptr;
    }
    // Get iterator of maneuver preceding or equal to time t
    const Maneuver &preceding_maneuver = std::prev(following_iterator)->second;
    // If maneuver has ended by or at time t, return nullptr,
    // otherwise ptr to maneuver.
    return preceding_maneuver.tf() <= t ? nullptr : &preceding_maneuver;
}


// private methods


void FlightPath::Calculate(const double t) const {
    // Calculates flight path until passed time, with time being
    // relative to System t0.

    // If a previous SegmentGroup has been left uncompleted,
    // finish it first.
    if (cache_.status.incomplete_element) {
        SegmentGroup *incomplete_group = last_group();
        cache_.status = incomplete_group->Calculate(t);
        // If time that calculation reached exceeds t,
        // calculation is complete.
        if (cache_.status.end_t > t) {
            return;
        }
    }
    // Add new groups and Calculate() them until end_t extends past
    // time t.
    while (cache_.status.end_t <= t) {
        // get maneuver (or non-maneuver) that new SegmentGroup will
        // correspond with.
        const Maneuver * const maneuver = FindManeuver(cache_.status.end_t);
        const Vector r = cache_.status.r;
        const Vector v = cache_.status.v;
        const double group_t = cache_.status.end_t;
        std::unique_ptr<SegmentGroup> group;
        if (maneuver == nullptr) {
            group = std::make_unique<BallisticSegmentGroup>(
                system_, r, v, group_t);
        } else {
            group = std::make_unique<ManeuverSegmentGroup>(
                system_, maneuver, r, v, group_t);
        }
        cache_.status = group->Calculate(t);
        cache_.groups[group_t] = std::move(group);
    }
}

const FlightPath::Segment& FlightPath::GetSegment(const double t) const {
    // validate t
    if (t < t0_) {
        throw std::invalid_argument(
            "FlightPath::GetSegment() passed invalid time: " +
            std::to_string(t) + " FlightPath begins at " + std::to_string(t0_));
    }
    // calculate segments for path until time t
    Calculate(t);
    // Get segment group for time t.
    SegmentGroup &group = *std::prev(cache_.groups.upper_bound(t))->second;
    // Get segment immediately before, or starting at time t.
    return group.GetSegment(t);
}

FlightPath::SegmentGroup* FlightPath::last_group() const {
    return cache_.groups.size() == 0 ?
        nullptr : &(*cache_.groups.rbegin()->second);
}

FlightPath::CalculationStatus FlightPath::calculation_status() const {
    if (cache_.status.end_t == -1.0) {
        cache_.status.end_t = t0_;
        cache_.status.r = r0_;
        cache_.status.v = v0_;
    }
    return cache_.status;
}

// FlightPath inner-classes -------------------------------------------

// SegmentGroup -------------------------------------------------------

FlightPath::SegmentGroup::SegmentGroup(
        const System &system, const Maneuver * const maneuver,
        const Vector r, const Vector v, double t):
        system_(system), maneuver_(maneuver), r_(r), v_(v), t_(t) {
    calculation_status_.r = r;
    calculation_status_.v = v;
    calculation_status_.end_t = t;
}

Orbit FlightPath::SegmentGroup::Predict(const double t) const {
    return GetSegment(t).Predict(t);
}

const FlightPath::Segment&
        FlightPath::SegmentGroup::GetSegment(const double t) const {
    const auto following_iterator = segments_.upper_bound(t);
    if (following_iterator == segments_.begin()) {
        throw std::invalid_argument(
            "FlightPath::SegmentGroup::GetSegment() : "
            "Passed time precedes first segment in SegmentGroup: " +
            std::to_string(t));
    }
    const auto iterator = std::prev(following_iterator);
    return *iterator->second;
}

FlightPath::CalculationStatus
        FlightPath::SegmentGroup::Calculate(const double t) {
    if (t < calculation_status_.end_t) {
        return calculation_status_;
    }
    // If the last segment has not finished being calculated,
    // continue calculating it until time t is reached or segment ends.
    if (calculation_status_.incomplete_element) {
        Segment &last_segment = *segments_.rbegin()->second;
        calculation_status_ = last_segment.Calculate(t);
        if (calculation_status_.end_t > t) {
            return calculation_status_;
        }
    }
    // Progress calculation of flight path until time t is reached or
    // SegmentGroup ends.
    while (calculation_status_.end_t <= t) {
        const Vector r = calculation_status_.r;
        const Vector v = calculation_status_.v;
        const double segment_time = calculation_status_.end_t;
        std::unique_ptr<Segment> segment = CreateSegment(r, v, segment_time);
        calculation_status_ = segment->Calculate(t);
        segments_[segment_time] = std::move(segment);
    }
    return calculation_status_;
}

// ManeuverSegmentGroup -----------------------------------------------

FlightPath::ManeuverSegmentGroup::ManeuverSegmentGroup(
        const System &system,
        const Maneuver * const maneuver,
        const Vector r, const Vector v, double t):
            SegmentGroup(system, maneuver, r, v, t) {}

std::unique_ptr<FlightPath::Segment>
        FlightPath::ManeuverSegmentGroup::CreateSegment(
        const Vector r, const Vector v, const double t) const {
    // todo
}

// BallisticSegmentGroup ----------------------------------------------

FlightPath::BallisticSegmentGroup::BallisticSegmentGroup(
        const System &system, const Vector r, const Vector v, double t):
            SegmentGroup(system, nullptr, r, v, t) {}

std::unique_ptr<FlightPath::Segment>
        FlightPath::BallisticSegmentGroup::CreateSegment(
        const Vector r, const Vector v, const double t) const {
    return std::move(std::make_unique<BallisticSegment>(system_, r, v, t));
}


}  // namespace kin
