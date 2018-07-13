cimport libcpp.memory as mem

from vector cimport Vector, PyVector
from orbit cimport Orbit, PyOrbit
from body cimport Body, PyBody
from system cimport System, PySystem


cdef extern from "path.h" namespace "kin::Maneuver" nogil:
    enum ManeuverType:
        kPrograde,
        kRetrograde,
        kNormal,
        kAntiNormal,
        kRadial,
        kAntiRadial,
        kFixed


cdef extern from "path.h" namespace "kin" nogil:
    struct KinematicData:
        Vector r, v


    cppclass PerformanceData:
        PerformanceData(double ve, double thrust)

        # Accessors.
        double ve() const
        double thrust() const
        double flow_rate() const


    cppclass OrbitData:
        const Orbit &orbit() const
        const Body &body() const


    cppclass Maneuver:

        # Constructors

        Maneuver(ManeuverType type,
                 double dv,
                 PerformanceData performance,
                 double m0,
                 double t0)

        Maneuver(Vector vector,
                 double dv,
                 PerformanceData performance,
                 double m0,
                 double t0)

        ManeuverType type() const
        double dv() const
        double m0() const
        double m1() const
        double t0() const
        double t1() const  # end time of maneuver.
        const PerformanceData& performance() const
        double duration() const
        double mass_fraction() const  # mass ratio 0-1 which is expended.
        double expended_mass() const  # propellant mass expended.

        double FindMassAtTime(const double t) const


    cppclass FlightPath:
        # Constructors
        FlightPath(
            const System &system,
            const Vector r,
            const Vector v,
            double t)

        # Gets KinematicData for passed point in time since t0 */
        KinematicData Predict(const double time) const
        OrbitData PredictOrbit(const double time, const Body *body) const
        const Maneuver *FindManeuver(const double t) const
        const Maneuver *FindNextManeuver(const double t) const
        void Add(const Maneuver &maneuver)
        bint ClearAfter(const double t)
        bint Remove(const Maneuver &maneuver)
        bint Clear()


cdef class PyKinematicData:
    cdef KinematicData _data

    cdef inline KinematicData val(self)

    @staticmethod
    cdef PyKinematicData cp(KinematicData data)


cdef class PyPerformanceData:
    cdef mem.unique_ptr[PerformanceData] _data

    cdef inline PerformanceData* get(self)

    @staticmethod
    cdef PyPerformanceData cp(PerformanceData data)


cdef class PyManeuver:
    cdef mem.unique_ptr[Maneuver] _maneuver

    @staticmethod
    cdef PyManeuver cp(Maneuver maneuver)

    cdef inline Maneuver* get(self)

    cpdef double find_mass_at_time(self, double t)


cdef class PyFlightPath:
    cdef FlightPath *_path
    cdef bint owning
    cdef PySystem _system  # Stores system to ensure proper deletion order.

    @staticmethod
    cdef inline PyFlightPath wrap(FlightPath *path):
        return PyFlightPath(ptr=<long long>path)

    cdef inline FlightPath* get(self):
        return self._path

    cpdef PyKinematicData predict(self, double time)
    # cpdef PredictOrbit(const double time, const Body *body = nullptr) const;
    cpdef PyManeuver find_maneuver(self, double t)
    cpdef PyManeuver find_next_maneuver(self, double t)

    cpdef void add(self, PyManeuver maneuver) except *
    cpdef bint clear(self)
    cpdef bint clear_after(self, double t)
    cpdef bint remove(self, PyManeuver maneuver)
