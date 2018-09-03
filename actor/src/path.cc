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
#include <algorithm>
#include "system.h"

namespace kin {


static constexpr double kMaxOrbitPeriodDurationPerStep      = 0.01;
static constexpr double kMinBallisticStepDuration           = 15.0;
static constexpr double kMaxMassRatioChangePerStep          = 0.001;

// Maneuver methods ---------------------------------------------------


Maneuver::Maneuver(ManeuverType type,
                   double dv,
                   const PerformanceData performance,
                   double m0,
                   double t0):
        type_(type), dv_(dv), performance_(performance), m0_(m0), t0_(t0) {}

Maneuver::Maneuver(const Vector vector,
                   double dv,
                   const PerformanceData performance,
                   double m0,
                   double t0):
        type_(kFixed), fixed_vector_(vector),
        dv_(dv), performance_(performance), m0_(m0), t0_(t0) {}

double Maneuver::mass_fraction() const {
    return 1 - std::pow(constants::e, -dv_ / performance_.ve());
}

double Maneuver::duration() const {
    return expended_mass() / performance_.flow_rate();
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
    // Find mass at passed t by subtracting expended mass from m0.
    return m0_ - (t - t0_) * performance_.flow_rate();
}

Vector Maneuver::FindThrustVector(
        const Body &ref, const Vector r, const Vector v, const double t) const {
    const Vector body_r = ref.PredictSystemPosition(t);
    const Vector body_v = ref.PredictSystemVelocity(t);
    const Vector rel_r = r - body_r;
    const Vector rel_v = v - body_v;
    switch (type_) {
        case kPrograde:
            return rel_v.normalized();
        case kRetrograde:
            return rel_v.normalized() * -1.0;
        case kRadial:
            return rel_r.normalized();
        case kAntiRadial:
            return rel_r.normalized() * -1.0;
        case kNormal:
            return rel_r.cross(rel_v).normalized();  // North when i is 0.0
        case kAntiNormal:
            return rel_v.cross(rel_r).normalized();  // South when i is 0.0
        case kFixed:
            return fixed_vector_;
    }
}


// FlightPath methods -------------------------------------------------


FlightPath::FlightPath(
    const System &system, const Vector r, const Vector v, double t):
        system_(system), r0_(r), v0_(v), t0_(t) {
    if (t < 0) {
        throw std::invalid_argument("FlightPath::FlightPath() : "
            "Passed value t (" + std::to_string(t) + ") was < 0");
    }
    // If position is 0, 0, 0, then orbits cannot be
    // properly calculated.
    if (r.squaredNorm() == 0.0) {
        throw std::invalid_argument("FlightPath::FlightPath() : "
            "Passed position r was [0,0,0]");
    }
    cache_.status = FlightPath::CalculationStatus(r, v, t);
}

KinematicData FlightPath::Predict(const double time) const {
    return GetSegment(time).Predict(time);
}

OrbitData FlightPath::PredictOrbit(
        const double time, const Body * const body) const {
    // If passed reference body is null, use body within
    // sphere of influence.
    if (body == nullptr) {
        return GetSegment(time).PredictOrbit(time);
    } else {
        // Produce orbit from current system position and velocity.
        const KinematicData kinematics = Predict(time);
        // Find position and velocity relative to reference body.
        const Vector rel_r = kinematics.r - body->PredictSystemPosition(time);
        const Vector rel_v = kinematics.v - body->PredictSystemVelocity(time);
        // Produce orbit from relative position, velocity, and body.
        return OrbitData(Orbit(*body, rel_r, rel_v), *body);
    }
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
    const Maneuver &preceding_maneuver = *std::prev(following_iterator)->second;
    // If maneuver has ended by or at time t, return nullptr,
    // otherwise ptr to maneuver.
    return preceding_maneuver.t1() <= t ? nullptr : &preceding_maneuver;
}

const Maneuver* FlightPath::FindNextManeuver(const double t) const {
    if (maneuvers_.size() == 0) {
        return nullptr;
    }
    auto following_iterator = maneuvers_.upper_bound(t);
    // If no following iterator exists, return nullptr
    if (following_iterator == maneuvers_.end()) {
        return nullptr;
    }
    return following_iterator->second.get();
}

void FlightPath::Add(const Maneuver &maneuver) {
    // Check that maneuver begins after all existing maneuvers
    // have ended.
    if (&maneuver == nullptr) {
        throw std::invalid_argument("FlightPath::Add() : "
            "Passed maneuver had null address.");
    }
    if (maneuvers_.size() > 0) {
        const Maneuver &last = *std::prev(maneuvers_.end())->second;
        if (last.t1() > maneuver.t0()) {
            throw std::invalid_argument("FlightPath::Add() : "
                "Passed Maneuver has t0 (" + std::to_string(maneuver.t0()) +
                ") that precedes the end of the last maneuver already in "
                "FlightPath (tf: " + std::to_string(last.t1()) + ")");
        }
    }
    maneuvers_[maneuver.t0()] = std::make_unique<Maneuver>(maneuver);  // Copy.
    ClearCache();  // Reset calculated data (calculated segments, etc).
}

bool FlightPath::Clear() {
    maneuvers_.clear();
    ClearCache();  // Reset calculated data (calculated segments, etc).
    return true;
}

bool FlightPath::ClearAfter(const double t) {
    // Clear maneuvers that begin after, but not at time t.
    const std::size_t initial_size = maneuvers_.size();
    std::map<double, std::unique_ptr<Maneuver> >::iterator first =
        maneuvers_.upper_bound(t);
    maneuvers_.erase(first, maneuvers_.end());
    ClearCache();  // Reset calculated data (calculated segments, etc).
    return maneuvers_.size() == initial_size;
}

bool FlightPath::Remove(const Maneuver &maneuver) {
    const std::size_t initial_size = maneuvers_.size();
    maneuvers_.erase(maneuver.t0());
    ClearCache();  // Reset calculated data (calculated segments, etc).
    return maneuvers_.size() == initial_size;
}


// Private methods


void FlightPath::Calculate(const double t) const {
    // Calculates flight path until passed time, with time being
    // relative to System t0.
    if (t < cache_.status.end_t) {
        return;
    }

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
        // Get maneuver (if any) that new SegmentGroup will
        // correspond with.
        const Maneuver * const maneuver = FindManeuver(cache_.status.end_t);
        const Vector r = cache_.status.r;
        const Vector v = cache_.status.v;
        const double group_t = cache_.status.end_t;
        std::unique_ptr<SegmentGroup> group;
        if (maneuver == nullptr) {
            const Maneuver * const next_maneuver = FindNextManeuver(
                    cache_.status.end_t);
            const double group_tf = next_maneuver == nullptr ?
                    -1.0 : next_maneuver->t0();
            group = std::make_unique<BallisticSegmentGroup>(
                system_, r, v, group_t, group_tf);
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

void FlightPath::ClearCache() const {
    cache_ = FlightPath::FlightPathCache();
    cache_.status = FlightPath::CalculationStatus(r0_, v0_, t0_);
}

FlightPath::SegmentGroup* FlightPath::last_group() const {
    return cache_.groups.size() == 0 ?
        nullptr : &(*cache_.groups.rbegin()->second);
}

FlightPath::CalculationStatus FlightPath::calculation_status() const {
    // If cache has not yet been initialized, do that now.
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

void FlightPath::Segment::CheckPredictionTime(const double t) const {
    if (t < 0) {
        throw std::invalid_argument(
            "FlightPath::Segment::CheckPredictionTime() : Passed t: " +
            std::to_string(t));
    }
    if (t < t0_) {
        throw std::invalid_argument(
            "FlightPath::Segment::CheckPredictionTime() : "
            "Passed t preceded start time of segment. t: " + std::to_string(t) +
            "start: " + std::to_string(t0_));
    }
}

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
    CheckPredictionTime(t);
    Calculate(t);
    // CheckPredictionTime only checks that t does not precede segment.
    if (t >= calculation_status_.end_t) {
        throw std::invalid_argument("FlightPath::ManeuverSegment::Predict() : "
            "Passed t was >= end time of segment. t: " + std::to_string(t) +
            " end: " + std::to_string(calculation_status_.end_t));
    }
    // A number of approximations are made here that reduce the
    // accuracy of the result, but which simplify the code,
    // and are faster to evaluate.
    // If accuracy becomes an issue, this method should be re-written.
    const double rel_t = t - t0_;
    const Vector r = r0_ + a_ * (std::pow(rel_t, 2) / 2);
    const Vector v = v0_ + a_ * rel_t;
    return {r, v};
}

OrbitData FlightPath::ManeuverSegment::PredictOrbit(const double t) const {
    // Predict system-relative position and velocity.
    const KinematicData kinematics = Predict(t);
    // Find position and velocity relative to reference body.
    const Vector rel_r = kinematics.r - primary_body_.PredictSystemPosition(t);
    const Vector rel_v = kinematics.v - primary_body_.PredictSystemVelocity(t);
    // Produce and return orbit data.
    return OrbitData(Orbit(primary_body_, rel_r, rel_v), primary_body_);
}

FlightPath::CalculationStatus
        FlightPath::ManeuverSegment::Calculate(const double t) const {
    // This method prepares the Segment to approximate the position and
    // velocity over the duration of the segment by approximating the
    // mean acceleration from thrust and gravity over the duration of
    // the segment.
    //
    // Duration is determined by arbitrary limits, determined by ratio
    // of segment duration to orbital period, and ratio of segment
    // duration to duration required to change actor mass by a
    // set amount. In addition, duration is also limited by the end
    // time of the maneuver.
    //
    // Acceleration due to thrust is determined by the acceleration at
    // the mean mass of the actor over the duration of the segment, and
    // the direction determined by maneuver.
    //
    // Acceleration due to gravity is determined by the acceleration
    // imposed by the primary body influencing the actor at the mean
    // position of the segment, ignoring gravitational effects.
    //
    // Approximate acceleration used in prediction of position is the
    // sum of thrust acceleration and gravitational acceleration.

    if (t < calculation_status_.end_t) {
        return calculation_status_;
    }
    const Orbit initial_orbit(primary_body_, r0_, v0_);
    // Attempt to determine when segment ends.

    const double duration_limit = [this, initial_orbit]() -> double {
        // Check first for duration in which maximum mass ratio
        // change occurs.
        const double delta_m = maneuver_.m0() * kMaxMassRatioChangePerStep;
        const double mass_limited_duration =
            delta_m / maneuver_.performance().flow_rate();
        // Check max duration of step allowed by ratio of orbital period.
        const double period_limited_duration =
            initial_orbit.period() * kMaxOrbitPeriodDurationPerStep;
        // Use smaller of the two duration limits.
        return std::min(mass_limited_duration, period_limited_duration);
    }();

    const double tf = std::min(t0_ + duration_limit, maneuver_.t1());
    const double duration = tf - t0_;

    // Approximate average acceleration over duration of step.
    const Vector thrust_a = [this, tf]() -> Vector {
        const double a0_mag = maneuver_.performance().thrust() / m0_;
        const double mf = maneuver_.FindMassAtTime(tf);
        const double a1_mag = maneuver_.performance().thrust() / mf;
        const double a_mag = (a0_mag * 2 + a1_mag) / 3;
        // Find acceleration vector
        // First find gravity independent position.
        return maneuver_.FindThrustVector(
            primary_body_, r0_, v0_, t0_) * a_mag;
    }();

    // Find what position would be halfway through burn if no
    // gravitational affects were applied.
    const double half_duration = duration / 2;
    const Vector gravity_independent_r1 =
        thrust_a / 2 * std::pow(half_duration, 2) + v0_ * half_duration + r0_;

    const Vector gravity_a =
            [this, gravity_independent_r1, half_duration]() -> Vector {
        // Find gravitational acceleration from mean position of
        // r0 and gravity-independent rf.
        // First, find relative position from primary body.
        const double mean_t =  t0_ + half_duration;
        const Vector body_r1 = primary_body_.PredictSystemPosition(mean_t);
        const Vector rel_r1 = gravity_independent_r1 - body_r1;
        const double mag = primary_body_.gm() / rel_r1.squaredNorm();
        return rel_r1.normalized() * -mag;
    }();

    // Set approximate acceleration used in segment.
    a_ = gravity_a + thrust_a;
    // Compute values needed in returned CalculationStatus.
    const Vector rf = r0_ + a_ / 2 * std::pow(duration, 2);
    const Vector vf = v0_ + a_ * duration;
    return calculation_status_ = CalculationStatus(rf, vf, tf, false);
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
    // Ensure that t does not come before segment.
    CheckPredictionTime(t);
    Orbit prediction = orbit_.Predict(t - t0_);
    // Produce system-relative kinematics by adding orbit
    // r and v, to body's system-relative r and v.
    KinematicData kinematics;
    kinematics.r = prediction.position() +
        primary_body_.PredictSystemPosition(t);
    kinematics.v = prediction.velocity() +
        primary_body_.PredictSystemVelocity(t);
    return kinematics;
}

OrbitData FlightPath::BallisticSegment::PredictOrbit(const double t) const {
    CheckPredictionTime(t);
    const Orbit prediction = orbit_.Predict(t - t0_);
    return OrbitData(prediction, primary_body_);
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
    // Get max step duration. This value may be reduced later.
    // If orbit is elliptical (e < 1) it should be some fraction of
    // an orbital period. Otherwise, roughly proportional to the
    // amount of time potentially required to escape the primary body's
    // sphere of influence.
    const double max_step_duration = orbit_.eccentricity() < 1.0 ?
            orbit_.period() * kMaxOrbitPeriodDurationPerStep :
            2 * PI / orbit_.mean_motion() * kMaxOrbitPeriodDurationPerStep;
    // Create array of bodies in sphere of influence,
    // and their max speed.
    // These bodies, which share the same parent as the segment, are
    // referred to here as peers.
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
        double step_duration = max_step_duration;
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
            double distance = position_difference.norm() -
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
                // Achilles and the tortoise logic.
                if (step_duration < kMinBallisticStepDuration) {
                    step_duration = kMinBallisticStepDuration;
                    break;  // There is no point in finding a closer peer body.
                }
            }
        }
        if (!(step_duration > 0.0)) {
            throw std::runtime_error("FlightPath::BallisticSegment::Calculate()"
                    " : step_duration was not > 0. value: " +
                    std::to_string(step_duration));
        }
        double new_t = step_t + step_duration;
        calculation_status_.end_t = new_t;
        // Advance calculation status
        const Orbit orbit_prediction = orbit_.Predict(new_t - t0_);
        const KinematicData local_data = orbit_prediction.kinematic_data();
        const KinematicData parent_body_system_data =
            primary_body_.PredictSystemKinematicData(new_t);
        const KinematicData system_data = local_data + parent_body_system_data;
        calculation_status_.r = system_data.r;
        calculation_status_.v = system_data.v;
        // If primary influence has changed,
        // this segment's end has been reached.
        const Body &new_primary =
            system_.FindPrimaryInfluence(system_data.r, new_t);
        if (new_primary.id() != primary_body_.id()) {
            break;
        }
    }
    return calculation_status_;
}

// SegmentGroup -------------------------------------------------------

FlightPath::SegmentGroup::SegmentGroup(
        const System &system, const Maneuver * const maneuver,
        const Vector r, const Vector v, const double t, const double tf):
        system_(system), maneuver_(maneuver), r_(r), v_(v), t_(t), tf_(tf) {
    if (t < 0) {
        throw std::invalid_argument("SegmentGroup::SegmentGroup() : "
            "Passed value t (" + std::to_string(t) + ") was < 0");
    }
    if (tf != -1.0 && tf <= t) {
        throw std::invalid_argument("SegmentGroup::SegmentGroup() : "
            "Passed value tf (" + std::to_string(t) + ") was < t "
            "(" + std::to_string(tf) + ")");
    }
    if (r.squaredNorm() == 0.0) {
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
        FlightPath::SegmentGroup::Calculate(double t) {
    // Note: This method is re-entrant but not thread-safe.

    // Check that passed time t is within bounds.
    if (t < t_) {
        throw std::invalid_argument("SegmentGroup::Calculate() : "
            "Passed value t (" + std::to_string(t) + ") was < group t0 ("
            + std::to_string(t_) + ")");
    }
    // Return early if calculation of segment group has already
    // surpassed t.
    if (t < calculation_status_.end_t) {
        return calculation_status_;
    }
    if (tf_ != -1.0 && t > tf_) {
        t = tf_;
    }
    // If the last segment has not finished being calculated,
    // continue calculating it until time t is reached or segment ends.
    if (calculation_status_.incomplete_element) {
        Segment &last_segment = *segments_.rbegin()->second;
        calculation_status_ = last_segment.Calculate(t);
    }
    // Progress calculation of flight path until time t is reached or
    // SegmentGroup ends.
    while (calculation_status_.end_t <= t &&
            (tf_ == -1 || calculation_status_.end_t < tf_)) {
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
    // Trim calculation status if calculation overran group end time.
    if (tf_ != -1.0 && calculation_status_.end_t > tf_) {
        // Get kinematic data for final time tf.
        const KinematicData tf_kinematics = Predict(tf_);
        calculation_status_ = CalculationStatus(
                tf_kinematics.r,
                tf_kinematics.v,
                tf_);
    } else if (tf_ == -1.0 || calculation_status_.end_t < tf_) {
        calculation_status_.incomplete_element = true;
    }
    return calculation_status_;
}

// ManeuverSegmentGroup -----------------------------------------------

FlightPath::ManeuverSegmentGroup::ManeuverSegmentGroup(
        const System &system,
        const Maneuver * const maneuver,
        const Vector r, const Vector v, const double t):
            SegmentGroup(system, maneuver, r, v, t, maneuver->t1()) {
    // validate input
    if (maneuver == nullptr) {
        throw std::invalid_argument(
            "ManeuverSegmentGroup::ManeuverSegmentGroup() : "
            "Passed maneuver was null");
    }
    if (maneuver->t0() != t) {
        throw std::invalid_argument(
            "ManeuverSegmentGroup::ManeuverSegmentGroup() : "
            "t: " + std::to_string(t) + " does not match maneuver t0: " +
            std::to_string(maneuver->t0()));
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
        const System &system,
        const Vector r, const Vector v, const double t, const double tf):
            SegmentGroup(system, nullptr, r, v, t, tf) {}

std::unique_ptr<FlightPath::Segment>
        FlightPath::BallisticSegmentGroup::CreateSegment(
        const Vector r, const Vector v, const double t) const {
    return std::move(std::make_unique<BallisticSegment>(system_, r, v, t));
}


}  // namespace kin
