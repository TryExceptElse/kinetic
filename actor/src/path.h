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
#include "util.h"

namespace kin {

class System;

// --------------------------------------------------------------------

/**
 * Structure containing information about an orbit.
 */
class OrbitData {
 public:
    OrbitData(const Orbit orbit, const Body &body):
        orbit_(orbit), body_(body) {}

    const Orbit &orbit() const { return orbit_; }
    const Body &body() const { return body_; }

    // Getters such as altitude, system_r, system_v, etc can be
    // added later.

 private:
    const Orbit orbit_;
    const Body &body_;
};

/**
 * Class containing information about maneuver performance.
 */
class PerformanceData {
 public:
    PerformanceData(double ve, double thrust): ve_(ve), thrust_(thrust) {}

    double ve() const { return ve_; }
    double thrust() const { return thrust_; }
    double flow_rate() const { return thrust_ / ve_; }

 private:
    double ve_;
    double thrust_;
};


// --------------------------------------------------------------------

// https://en.wikipedia.org/wiki/Tsiolkovsky_rocket_equation
class Maneuver {
 public:
    /**
     * Enum defining the type of maneuver being performed.
     * This will determine the direction of acceleration over time.
     */
    enum ManeuverType {
        kPrograde, kRetrograde, kNormal, kAntiNormal, kRadial, kAntiRadial,
        kFixed
    };

    Maneuver(ManeuverType type,
             double dv,
             const PerformanceData performance,
             double m0,
             double t0);

    /**
     * Alternative Maneuver constructor that creates a Maneuver of a fixed
     * vector
     */
    Maneuver(const Vector vector,
             double dv,
             const PerformanceData performance,
             double m0,
             double t0);

    ManeuverType type() const { return type_; }
    double dv() const { return dv_; }
    double m0() const { return m0_; }
    double m1() const { return m0_ - expended_mass(); }
    double t0() const { return t0_; }
    double t1() const { return t0_ + duration(); }  // end time of maneuver.
    const PerformanceData& performance() const { return performance_; }
    double duration() const;
    double mass_fraction() const;  // mass ratio 0-1 which is expended.
    double expended_mass() const;  // propellant mass expended.

    double FindMassAtTime(const double t) const;

    /**
     * Finds normalized vector indicating maneuver's direction of
     * thrust at a given position and velocity, with a given reference
     * body.
     * Expects position and velocity vectors to be relative to a
     * reference Body.
     * Returns vector relative to reference body.
     */
    Vector FindThrustVector(
        const Body &ref, const Vector r, const Vector v, const double t) const;

 private:
    ManeuverType type_;  // type of maneuver
    Vector fixed_vector_;  // Used when vector is fixed, otherwise ignored.
    double dv_;  // delta-V of maneuver
    PerformanceData performance_;
    double m0_;
    double t0_;
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

    /** Gets KinematicData for passed point in time since t0 */
    KinematicData Predict(const double time) const;

    /**
     * Gets OrbitData for passed point in time since t0.
     *
     * Returned orbit will not match true trajectory if the passed
     * body is not the primary influence on the flightpath at
     * passed time.
     *
     * The returned orbit will be highly inaccurate if the passed
     * body's sphere of influence does not include the flightpath at
     * the passed time.
     */
    OrbitData PredictOrbit(const double time, const Body *body = nullptr) const;

    /**
     * Gets pointer to maneuver at passed time or else nullptr.
     * Maneuver start time is inclusive, and end time is not.
     *
     * If maneuver A ends at time 5, and time 5 is passed as an
     * argument, it will not be returned by this method.
     */
    const Maneuver *FindManeuver(const double t) const;

    /**
     * Gets pointer to next maneuver with a start time following passed
     * time, or nullptr if no following maneuver exists.
     *
     * If the passed time is contained within a maneuver, a pointer to
     * the next maneuver will be returned if one exists, otherwise
     * a nullptr will be returned.
     *
     * If maneuver A starts at time 5, and 5 is passed as an argument,
     * maneuver A will not be returned by this method.
     */
    const Maneuver *FindNextManeuver(const double t) const;

    /**
     * Adds a maneuver to FlightPath at the specified time.
     * Added maneuver must have a t0 that is later than the end time
     * of any existing maneuvers in the FlightPath.
     *
     * Added maneuver is copied, no reference to the passed maneuver
     * is stored within FlightPath.
     */
    void Add(const Maneuver &maneuver);

    /**
     * Removes all maneuvers currently in FlightPath
     */
    bool Clear();

    /**
     * Removes all maneuvers in FlightPath with a t0 after passed time.
     */
    bool ClearAfter(const double t);

    /**
     * Removes specified maneuver.
     */
    bool Remove(const Maneuver &maneuver);

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
        CalculationStatus(
            const Vector r,
            const Vector v,
            double end_t,
            bool incomplete = false):
                end_t(end_t), r(r), v(v), incomplete_element(incomplete) {}

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

    std::map<double, std::unique_ptr<Maneuver> > maneuvers_;
    // Raw pointer should never be invalid when used as intended;
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
     * This method may have to calculate
     * additional orbital segments before it can return the
     * desired Segment.
     */
    const Segment& GetSegment(const double t) const;

    /**
     * Clears cached data and restores cache to default state.
     * This is intended to be used when path maneuvers are changed.
     * or other events occur that may change path trajectory
     */
    void ClearCache() const;  // Only mutable members changed.

    // private getters

    /** Gets last group in cache */
    SegmentGroup* last_group() const;
    CalculationStatus calculation_status() const;

    // Nested Classes -------------------------------------------------

    /**
     * A Segment stores information about a limited portion of a path
     * for which a non-iterative method of finding position and
     * velocity exists.
     *
     * Each segment has only a single primary influence, and so a
     * Segment must end when it moves into a different primary sphere
     * of influence.
     */
    class Segment {
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

        virtual ~Segment() {}

        /**
         * Predicts orbital trajectory at passed time.
         * time is relative to universe t0.
         *
         * Returned KinematicData is relative to system.
         */
        virtual KinematicData Predict(const double t) const = 0;

        virtual OrbitData PredictOrbit(const double t) const = 0;

        /**
         * Calculates flight path until passed time t or Segment ends.
         */
        virtual CalculationStatus Calculate(const double t) const = 0;

     protected:
        const System &system_;
        const Body &primary_body_;
        const Vector r0_;
        const Vector v0_;
        const double t0_;
        mutable CalculationStatus calculation_status_;

        /**
         * Checks that passed time t is >=0 and does not precede start
         * of segment.
         */
        void CheckPredictionTime(const double t) const;
    };

    // ----------------------------------------------------------------

    class ManeuverSegment: public Segment {
     public:
        ManeuverSegment(
            const System &system,
            const Maneuver &maneuver,
            const Vector r,
            const Vector v,
            double t);

        KinematicData Predict(const double t) const;
        OrbitData PredictOrbit(const double t) const;
        CalculationStatus Calculate(const double t) const;

     private:
        const Maneuver &maneuver_;
        const double m0_;               // Mass at beginning of segment.
        mutable Vector a_;              // Acceleration used for approximation.
    };

    // ----------------------------------------------------------------

    class BallisticSegment: public Segment {
     public:
        BallisticSegment(
            const System &system,
            const Vector r,
            const Vector v,
            double t);

        KinematicData Predict(const double t) const;
        OrbitData PredictOrbit(const double t) const;
        CalculationStatus Calculate(const double t) const;

     private:
        Orbit orbit_;
        bool calculation_complete_;
    };

    // ----------------------------------------------------------------

    /**
     * Grouping of Segments grouped by the maneuver that they take
     * place during.
     */
    class SegmentGroup {
     public:
        SegmentGroup(const System &system, const Maneuver * const maneuver,
            const Vector r, const Vector v, double t, double tf = -1.0);

        virtual ~SegmentGroup() {}

        /** Gets Orbit object for passed time relative to universe t0. */
        KinematicData Predict(const double t) const;

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
        const Maneuver* maneuver() const { return maneuver_; }

     protected:
        const System &system_;
        const Maneuver * const maneuver_;
        const Vector r_;
        const Vector v_;
        const double t_;
        const double tf_;
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
            const Vector r, const Vector v, double t, double tf = -1.0);

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
