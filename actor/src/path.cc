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

#include "path.h"

#include <iterator>
#include <vector>
#include <utility>  // pair
#include <stdexcept>
#include "system.h"

namespace kin {


static constexpr double kMaxOrbitPeriodDurationPerStep      = 0.01;
static constexpr double kMinBallisticStepDuration           = 15.0;
static constexpr double kMaxMassRatioChangePerStep          = 0.001;
static constexpr double kMinManeuverSegmentLen              = 0.06;

// Maneuver methods ---------------------------------------------------


Maneuver::Maneuver(ManeuverType type,
                   double dv,
                   const PerformanceData performance,
                   double m0,
                   double t0):
        type_(type), dv_(dv), performance_(performance), m0_(m0), t0_(t0) {}

double Maneuver::mass_fraction() const {
    return 1 - std::pow(e, -dv_ / performance_.ve());
}

double Maneuver::duration() const {
    return mass_fraction() / performance_.flow_rate();
}

double Maneuver::expended_mass() const {
    return m0_ * mass_fraction();
}

double Maneuver::FindMassAtTime(const double t) const {
    // validate
    if (t < t0_ || t > t1()) {
        throw std::invalid_argument(
            "Maneuver::FindMassAtTime() : "
            "Passed time t: " + std::to_string(t) +
            " outside Maneuver time range: " +
            std::to_string(t0_) + " to " + std::to_string(t1()));
    }
    // find mass at passed t by
    return m0_ - (t - t0_) * performance_.flow_rate();
}


// FlightPath methods -------------------------------------------------


FlightPath::FlightPath(
    const System &system, const Vector r, const Vector v, double t):
        system_(system), r0_(r), v0_(v), t0_(t) {
    if (t < 0) {
        throw std::invalid_argument("FlightPath::FlightPath() : "
            "Passed value t (" + std::to_string(t) + ") was < 0");
    }
    if (r.sqlen() == 0.0) {
        throw std::invalid_argument("FlightPath::FlightPath() : "
            "Passed position r was [0,0,0]");
    }
}

KinematicData FlightPath::Predict(const double time) const {
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
    return preceding_maneuver.t1() <= t ? nullptr : &preceding_maneuver;
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

// Segment ------------------------------------------------------------

FlightPath::Segment::Segment(
        const System &system, const Vector r, const Vector v, double t):
    system_(system),
    primary_body_(system.FindPrimaryInfluence(r, t)),
    r0_(r),
    v0_(v),
    t0_(t) {}

// ManeuverSegment ----------------------------------------------------

FlightPath::ManeuverSegment::ManeuverSegment(
        const System &system,
        const Maneuver &maneuver,
        const Vector r,
        const Vector v,
        double t):
    Segment(system, r, v, t),
    maneuver_(maneuver),
    m0_(maneuver.FindMassAtTime(t)) {}

KinematicData FlightPath::ManeuverSegment::Predict(const double t) const {
    if (t < 0) {
        std::invalid_argument(
            "FlightPath::ManeuverSegment::Predict() : Passed t: " +
            std::to_string(t));
    }
    Calculate(t);
    // todo
}

FlightPath::CalculationStatus
        FlightPath::ManeuverSegment::Calculate(const double t) const {
    if (t < calculation_status_.end_t) {
        return calculation_status_;
    }
    // Attempt to determine when segment ends.
    throw std::runtime_error(""); // PLACEHOLDER
}

// BallisticSegment ---------------------------------------------------

FlightPath::BallisticSegment::BallisticSegment(
        const System &system,
        const Vector r,
        const Vector v,
        double t):
            Segment(system, r, v, t),
            orbit_(primary_body_, r, v) {}

KinematicData FlightPath::BallisticSegment::Predict(const double t) const {
    if (t < t0_) {
        std::invalid_argument(
            "FlightPath::ManeuverSegment::Predict() : Passed t < t0: " +
            std::to_string(t));
    }
    Orbit prediction = orbit_.Predict(t - t0_);
    KinematicData kinematics;
    kinematics.r = prediction.position() +
        primary_body_.PredictSystemPosition(t);
    kinematics.v = prediction.velocity() +
        primary_body_.PredictSystemVelocity(t);
    return kinematics;
}

FlightPath::CalculationStatus
        FlightPath::BallisticSegment::Calculate(const double t) const {
    if (t < calculation_status_.end_t) {
        return calculation_status_;
    }
    // if no peer-bodies exist as children under parent
    // and orbit never exceeds sphere of influence,
    // then segment will not have any end.
    if (primary_body_.children().size() == 0 &&
            orbit_.eccentricity() < 1.0 &&
            orbit_.apoapsis() < primary_body_.sphere_of_influence()) {
        CalculationStatus status;
        Orbit orbit_at_end_t = orbit_.Predict(t + 1.0 - t0_);
        status.end_t = t + 1.0;
        status.r = orbit_at_end_t.position();
        status.v = orbit_at_end_t.velocity();
        calculation_status_ = status;
        return calculation_status_;
    }
    // Create array of bodies in sphere of influence,
    // and their max speed.
    // These bodies, which share the same parent as the segment, are
    // referred to here as peers.
    const double maxStepDuration =
        orbit_.period() * kMaxOrbitPeriodDurationPerStep;
    std::vector<std::pair<Body*, double> > peer_body_speeds;
    for (const BodyIdPair &body_id_pair : primary_body_.children()) {
        // sanity check
        if (body_id_pair.second->parent()->id() != primary_body_.id()) {
            throw std::runtime_error("FlightPath::BallisticSegment::Calculate()"
                " : Peer body's parent had different ID from BallisticSegment");
        }
        const std::pair<Body*, double> body_speed_pair(
            body_id_pair.second.get(),
            body_id_pair.second->orbit()->max_speed());
        peer_body_speeds.push_back(body_speed_pair);
    }
    while (calculation_status_.end_t <= t) {
        double step_t = calculation_status_.end_t;
        // Find duration of step.
        double step_duration = maxStepDuration;
        // Reduce step_duration if a peer sphere of influence may
        // be intersected.
        // Find smallest time-separation between predicted position and any
        // spheres of influence of bodies orbiting the same parent
        // (Referred to here as peers).
        for (std::pair<Body*, double> body_speed_pair : peer_body_speeds) {
            const Body * const body = body_speed_pair.first;
            Vector local_position = orbit_.Predict(step_t - t0_).position();
            Vector local_peer_position = body->PredictLocalPosition(step_t);
            Vector position_difference = local_position - local_peer_position;
            double distance = position_difference.len() -
                body->sphere_of_influence();
            // Sanity check.
            if (distance < 0) {
                throw std::runtime_error(
                    std::string("FlightPath::BallisticSegment::Calculate()") +
                    " : Distance to peer-body " + std::string(body->id()) +
                    " < 0.");
            }
            double time_separation = distance / body_speed_pair.second;
            if (time_separation < step_duration) {
                step_duration = time_separation;
                // Enforce minimum step duration to avoid zeno's
                // achilles logic.
                if (step_duration < kMinBallisticStepDuration) {
                    step_duration = kMinBallisticStepDuration;
                    break;  // There is no point in finding a closer peer body.
                }
            }
        }
        double new_t = step_t + step_duration;
        calculation_status_.end_t = step_t + step_duration;
        // Advance calculation status
        const Orbit orbit_prediction = orbit_.Predict(new_t - t0_);
        const Vector local_position = orbit_prediction.position();
        const Vector local_velocity = orbit_prediction.velocity();
        const Vector system_position =
            local_position + primary_body_.PredictSystemPosition(new_t);
        const Vector system_velocity =
            local_velocity + primary_body_.PredictSystemVelocity(new_t);
        calculation_status_.r = system_position;
        calculation_status_.v = system_velocity;
        // If primary influence has changed,
        // this segment's end has been reached.
        const Body &new_primary =
            system_.FindPrimaryInfluence(system_position, new_t);
        if (new_primary.id() != primary_body_.id()) {
            break;
        }
    }
    return calculation_status_;
}

// SegmentGroup -------------------------------------------------------

FlightPath::SegmentGroup::SegmentGroup(
        const System &system, const Maneuver * const maneuver,
        const Vector r, const Vector v, double t):
        system_(system), maneuver_(maneuver), r_(r), v_(v), t_(t) {
    if (t < 0) {
        throw std::invalid_argument("SegmentGroup::SegmentGroup() : "
            "Passed value t (" + std::to_string(t) + ") was < 0");
    }
    if (r.sqlen() == 0.0) {
        throw std::invalid_argument("SegmentGroup::SegmentGroup() : "
            "Passed position r was [0,0,0]");
    }
    calculation_status_.r = r;
    calculation_status_.v = v;
    calculation_status_.end_t = t;
}

KinematicData FlightPath::SegmentGroup::Predict(const double t) const {
    return GetSegment(t).Predict(t);
}

const FlightPath::Segment&
        FlightPath::SegmentGroup::GetSegment(const double t) const {
    if (segments_.size() == 0) {
        throw std::runtime_error("SegmentGroup::GetSegment() : "
            "No segments present.");
    }
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
        // Check to prevent infinite loops. An error is preferable.
        if (calculation_status_.end_t <= segment_time) {
            throw std::runtime_error("SegmentGroup::Calculate() : "
                "Calculation of segment did not result in later end-time");
        }
        segments_[segment_time] = std::move(segment);
    }
    return calculation_status_;
}

// ManeuverSegmentGroup -----------------------------------------------

FlightPath::ManeuverSegmentGroup::ManeuverSegmentGroup(
        const System &system,
        const Maneuver * const maneuver,
        const Vector r, const Vector v, double t):
            SegmentGroup(system, maneuver, r, v, t) {
    // validate input
    if (maneuver == nullptr) {
        throw std::invalid_argument(
            "ManeuverSegmentGroup::ManeuverSegmentGroup() : "
            "Passed maneuver was null");
    }
    if (maneuver->t0() != t) {
        throw std::invalid_argument(
            "t: " + std::to_string(t) + " does not match maneuver t0: " +
            std::to_string(t));
    }
}

std::unique_ptr<FlightPath::Segment>
        FlightPath::ManeuverSegmentGroup::CreateSegment(
        const Vector r, const Vector v, const double t) const {
    return std::move(std::make_unique<ManeuverSegment>(
        system_, *maneuver_, r, v, t));
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
