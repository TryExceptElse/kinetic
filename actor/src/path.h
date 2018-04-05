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
    Maneuver(const ManeuverType type, double dv);

    ManeuverType type() const { return type_; }
    double dv() const { return dv_; }
    double tf() const;

 private:
    ManeuverType type_;  // type of maneuver
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

    /**
     * Gets pointer to maneuver at passed time or else nullptr.
     * Maneuver start time is inclusive, and end time is not.
     * If maneuver A ends at time 5, and time 5 is passed as an
     * argument, it will not be returned by this method.
     */
    const Maneuver *FindManeuver(const double t) const;  // todo

 private:
    // forward declared nested classes  (declared in full below)

    class Segment;
    class ManeuverSegment;
    class BallisticSegment;
    class SegmentGroup;
    class ManeuverSegmentGroup;
    class BallisticSegmentGroup;

    /** Used to return results of flight path calculation */
    struct CalculationStatus {
        CalculationStatus(): end_t(-1.0), incomplete_element(false) {}

        double end_t;  // Time at which evaluation of segment ended.
        Vector r;      // Final position of calculation segment.
        Vector v;      // Final velocity of calculation segment.
        bool incomplete_element;  // last element calculated was unfinished.
    };

    /**
     * Used to store information that will be replaced when
     * maneuvers or other information changes
     */
    struct FlightPathCache {
        // FlightPath maintains a FlightPathCache
        // which in turn contains SegmentGroups associated with each
        // burn or coast period
        // which in turn contain path segments.
        std::map<double, std::unique_ptr<SegmentGroup> > groups;
        // stores result of last path calculation
        CalculationStatus status;
    };

    // members

    std::map<double, Maneuver> maneuvers_;
    // raw pointer should never be invalid when used as intended;
    // system owns actor, which owns path. If system is destroyed,
    // so is FlightPath.
    const System &system_;
    const Vector r0_;  // position relative to system origin
    const Vector v0_;  // velocity relative to system
    const double t0_;  // start time of flight path relative to system
    mutable FlightPathCache cache_;

    /**
     * Calculate path segments from current time until passed time t.
     * time range is inclusive; Ie: Path information at time t should
     * be able to be retrieved after Calculate(t) is called.
     */
    void Calculate(const double t) const;

    /**
     * Get Segment of orbit which describes position at time t.
     * This method is not constant because it may have to calculate
     * additional orbital segments before it can return the
     * desired Segment.
     */
    const Segment& GetSegment(const double t) const;

    // private getters

    /** Gets last group in cache */
    SegmentGroup* last_group() const;
    CalculationStatus calculation_status() const;

    // Nested Classes -------------------------------------------------

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
        virtual Orbit Predict(const double t) const;

        /**
         * Calculates flight path until passed time t or Segment ends.
         */
        virtual CalculationStatus Calculate(const double t);
     protected:
        const System &system_;
        const Vector r0_;
        const Vector v0_;
        double t0_;
    };

    // ----------------------------------------------------------------

    class ManeuverSegment: public Segment {
     public:
        ManeuverSegment(
            const System &system, const Vector r, const Vector v, double t);
    };

    // ----------------------------------------------------------------

    class BallisticSegment: public Segment {
     public:
        BallisticSegment(
            const System &system, const Vector r, const Vector v, double t);
    };

    // ----------------------------------------------------------------

    /**
     * Grouping of Segments grouped by the maneuver that they take
     * place during.
     */
    class SegmentGroup {
     public:
        SegmentGroup(const System &system, const Maneuver * const maneuver,
            const Vector r, const Vector v, double t);

        /** Gets Orbit object for passed time relative to universe t0. */
        Orbit Predict(const double t) const;

        /** Gets segment that includes passed time t. */
        const Segment& GetSegment(const double t) const;

        /**
         * Calculates flightpath until passed time t relative to
         * system t0.
         */
        CalculationStatus Calculate(const double t);

        // getters
        const std::map<double, std::unique_ptr<Segment> >& segments() const {
            return segments_;
        }
        const Maneuver * const maneuver() const { return maneuver_; }

     protected:
        const System &system_;
        const Maneuver * const maneuver_;
        const Vector r_;
        const Vector v_;
        const double t_;
        std::map<double, std::unique_ptr<Segment> > segments_;
        CalculationStatus calculation_status_;

        /**
         * Constructs new segment to be added to group.
         * Intended to be called within Calculate().
         */
        virtual std::unique_ptr<Segment> CreateSegment(
                const Vector r, const Vector v, const double t) const = 0;
    };

    // ----------------------------------------------------------------

    class ManeuverSegmentGroup: public SegmentGroup {
     public:
        ManeuverSegmentGroup(const System &system,
            const Maneuver * const maneuver,
            const Vector r, const Vector v, double t);

        /**
         * Constructs new segment to be added to group.
         * Intended to be called within Calculate().
         */
        std::unique_ptr<Segment> CreateSegment(
                const Vector r, const Vector v, const double t) const;
    };

    // ----------------------------------------------------------------

    class BallisticSegmentGroup: public SegmentGroup {
     public:
        BallisticSegmentGroup(const System &system,
            const Vector r, const Vector v, double t);

        /**
         * Constructs new segment to be added to group.
         * Intended to be called within Calculate().
         */
        std::unique_ptr<Segment> CreateSegment(
                const Vector r, const Vector v, const double t) const;
    };

    // ----------------------------------------------------------------

};


// --------------------------------------------------------------------


}  // namespace kin

#endif  // ACTOR_SRC_PATH_H_
