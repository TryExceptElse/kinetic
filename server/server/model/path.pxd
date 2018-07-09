cimport libcpp.memory as mem

from vector cimport Vector, PyVector


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

    # TODO:
