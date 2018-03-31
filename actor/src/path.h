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

#ifndef ACTOR_SRC_PATH_H_
#define ACTOR_SRC_PATH_H_

#include <map>
#include <memory>
#include "vector.h"
#include "orbit.h"

namespace kin {

class System;

// --------------------------------------------------------------------

/**
 * Enum defining the type of maneuver being performed.
 * This will determine the direction of acceleration over time.
 */
enum ManeuverType {
    kPrograde, kRetrograde, kNormal, kAntiNormal, kRadial, kAntiRadial, kFixed
};


// --------------------------------------------------------------------


class Maneuver {
 public:
    Maneuver(const ManeuverType type, const Vector r0, const Vector p0);

    Vector r0() const { return r0_; }
    Vector v0() const { return v0_; }
    double dv() const { return dv_; }
    double tf() const;

 private:
    ManeuverType type_;  // type of maneuver
    Vector r0_;  // initial position relative to body
    Vector p0_;  // initial velocity relative to body
    double dv_;  // delta-V of maneuver
};


// --------------------------------------------------------------------


/**
 * A FlightPath is a series of Maneuvers and Trajectories, which taken
 * together allow the position and velocity at any time between t0 and
 * tf to be determined.
 */
class FlightPath {
 public:
    FlightPath(const System &system, const Vector r, const Vector v, double t);

    /** Gets Orbit object for passed point in time since t0 */
    Orbit Predict(const double time) const;

    /** Gets pointer to maneuver at passed time or else nullptr */
    const Maneuver *FindManeuver(const double t) const;  // todo

    /** Gets the next maneuver after passed time */
    const Maneuver& NextManeuver(const double t) const;  // todo

 private:
    // forward declared nested classes  (declared in full below)

    class Segment;
    class ManeuverSegment;
    class BallisticSegment;

    struct FlightPathCache;
    struct InstantaneousStatus;

    // members

    std::map<double, Maneuver> maneuvers_;
    // raw pointer should never be invalid when used as intended;
    // system owns actor, which owns path. If system is destroyed,
    // so is FlightPath.
    const System &system_;
    const Vector r0_;  // position relative to system origin
    const Vector v0_;  // velocity relative to system
    const double t0_;  // start time of flight path relative to system
    double current_time_;  // time relative to universe t0
    bool cache_initialized_;
    FlightPathCache cache_;

    /** Calculate path segments from current time until passed time t */
    void Calculate(const double time);

    /**
     * Calculate path from passed start time until end time,
     * or maneuver changes.
     * Returns -1.0 if no maneuver change occurred in time range,
     * otherwise returns time at which maneuver status changed.
     * Calculation ends.
     * Helper method for Calculate().
     */
    InstantaneousStatus CalculateUntilManeuverChange(
        const InstantaneousStatus start_status, const double end);

    /** Get iterator to passed time t */
    std::map<double, std::unique_ptr<Segment> >::iterator
            GetSegmentIterator(const double t) const;

    /** Get Segment of orbit which describes position at time t */
    const Segment& GetSegment(const double t) const;

    /** Gets current maneuver if one exists

    /**
     * Removes segments from cache_ that precede current time and
     * are unused.
     */
    void CleanSegments();

    // Nested Classes -------------------------------------------------


    struct FlightPathCache { // todo
        FlightPathCache(): calc_t(0.0) {}

        double calc_t;
        std::map<double, std::unique_ptr<Segment> > segments;
    }

    /**
     * Structure containing information about path at specified
     * instant in time.
     */
    struct InstantaneousStatus { // todo
        InstantaneousStatus(
            double time, Maneuver *maneuver, Vector r, Vector v);

        const double time;  // relative
        const Maneuver * const maneuver;
        const Vector r;
        const Vector v;
    }

    // ----------------------------------------------------------------

    /**
     * A Segment stores information about a limited portion of a path
     * for which a non-iterative method of finding position and
     * velocity exists.
     */
    class Segment {  // todo
     public:
        /**
         * Creates segment, that begins at position r, with velocity v,
         * at time t.
         *
         * r is relative to the system origin
         * v is relative to the system motion
         * t is relative to universe t0.
         */
        Segment(const System &system, const Vector r, const Vector v, double t);

        /**
         * Predicts orbital trajectory at passed time.
         * time is relative to universe t0.
         */
        virtual Orbit Predict(const double t);

        /**
         * Attempts to find the end of this segment between start_t and
         * end_t, inclusive.
         * Returns time at which segment ends, or -1.0 if end was
         * not found.
         */
        virtual double FindEnd(const double start_t, const double end_t);
     private:
        const System &system_;
        const Vector r0_;
        const Vector v0_;
        double t0_;
    }

    // ----------------------------------------------------------------

    class ManeuverSegment: public Segment {
     public:
        ManeuverSegment(
            const System &system, const Vector r, const Vector v, double t);
    }

    // ----------------------------------------------------------------

    class BallisticSegment: public Segment {
     public:
        BallisticSegment(
            const System &system, const Vector r, const Vector v, double t);
    }

};


// --------------------------------------------------------------------


}  // namespace kin

#endif  // ACTOR_SRC_PATH_H_
