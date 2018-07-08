cimport libcpp.memory as mem

from vector cimport Vector, PyVector


cdef extern from "path.h" namespace "kin" nogil:
    cdef struct KinematicData:
        Vector r, v


    cdef cppclass PerformanceData:
        PerformanceData(double ve, double thrust)

        # Accessors.
        double ve() const
        double thrust() const
        double flow_rate() const


    cdef cppclass Maneuver:
        enum ManeuverType:
            kPrograde, kRetrograde, kNormal, kAntiNormal, kRadial, kAntiRadial,
            kFixed

        # Constructors

        Maneuver(ManeuverType type,
                 double dv,
                 const PerformanceData data,
                 double m0,
                 double t0)

        Maneuver(const Vector vector,
                 double dv,
                 const PerformanceData data,
                 double m0,
                 double t0);


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
