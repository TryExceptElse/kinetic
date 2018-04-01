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


// FlightPath methods -------------------------------------------------


FlightPath::FlightPath(
    const System &system, const Vector r, const Vector v, double t):
        system_(system), r0_(r), v0_(v), t0_(t) {}

Orbit FlightPath::Predict(const double time) const {
    return GetSegment(time).Predict(time);
}

const Maneuver* FindManeuver(const double t) const {
    if (maneuvers_.size() == 0) {
        return nullptr;
    }
    auto following_iterator = maneuvers_.upper_bound(t);
    // If no previous iterator exists, return nullptr
    if (following_iterator == maneuvers_.begin()) {
        return nullptr;
    }
    // Get iterator of maneuver preceding or equal to time t
    Maneuver &preceding_maneuver = std::prev(following_iterator)->second;
    // If maneuver has ended by time t, return nullptr,
    // otherwise ptr to maneuver.
    return preceding_maneuver.tf() <= t ? nullptr : &preceding_maneuver;
}

const Maneuver& NextManeuver(const double t) const {
}


// private methods


void FlightPath::Calculate(const double time) {
    // Calculates flight path until passed time, with time being
    // relative to FlightPath t0.
    if (time <= cache_.calc_t) {
        return;  // No calculation needed.
    }
    // remove segments that are no longer needed.
    CleanSegments();
    // generate segments from current time until passed time t.
    double calc_t = cache_.calc_t;  // latest calculated time
}

InstantaneousStatus FlightPath::CalculateUntilManeuverChange(
        const double start, const double end) {
    // get maneuver at start time, then proceed until end time is
    // reached, or maneuver changes.
    const Maneuver *maneuver = FindManeuver(start);
    // get starting segment
    const Segment *segment = &GetSegment(calc_t);
    // if segment started before maneuver
    while (calc_t < time) {
        const double segment_end_t = segment->FindEnd(calc_t, time);
        if (segment_end_t == -1.0) {
            // segment does not end in calculated time span.
            cache_.calc_t = time;
        } else {
            // segment ends; create new segment and continue
            const Orbit latest_orbit = segment->Predict(segment_end_t)
            const Vector latest_r = orbit.position();
            const Vector latest_v = orbit.velocity();
        }
    }
}

void FlightPath::CleanSegments() {
    auto first_needed_iterator = GetSegmentIterator(current_time_);
    cache_.segments.erase(cache_.segments.begin(), first_needed_iterator);
}

std::map<double, std::unique_ptr<Segment> >::iterator
        FlightPath::GetSegmentIterator(const double t) const {
    // validate t
    if (t < t0_) {
        throw std::invalid_argument(
            "FlightPath::GetSegment() passed invalid time: " + t +
            " FlightPath begins at " + t0_);
    }
    // calculate segments for path until time t
    Calculate(t);
    // Get segment immediately before, or starting at time t.
    return std::prev(cache_.segments.upper_bound(t));
}

const Segment& FlightPath::GetSegment(const double t) const {
    return *GetSegmentIterator(t);
}


// --------------------------------------------------------------------


}  // namespace kin
