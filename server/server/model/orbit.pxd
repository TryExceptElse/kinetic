cimport cython as cy

from server.model.body cimport Body, PyBody
from server.model.vector cimport Vector, PyVector


cdef extern from "path.h" namespace "kin" nogil:
    cppclass Orbit:
        Orbit(const Body &ref,
            double a, double e, double i, double l, double w, double t)

        Orbit(double u, double a, double e, double i,
              double l, double w, double t)

        Orbit(const Body &ref, const Vector r, const Vector v)

        Orbit(double u, const Vector r, const Vector v)

        Orbit(const Orbit &other)

        double gravitational_parameter() const
        double semi_major_axis() const
        double periapsis() const
        double apoapsis() const
        double eccentricity() const
        double inclination() const
        double longitude_of_ascending_node() const
        double argument_of_periapsis() const
        double true_anomaly() const
        double semiparameter() const
        double semi_minor_axis() const
        double period() const
        double eccentric_anomaly() const
        double mean_anomaly() const
        double mean_motion() const
        double time_since_periapsis() const
        double min_speed() const
        double max_speed() const
        Vector position() const
        Vector velocity() const

        # Vectors in geocentric equatorial inertial coordinates
        void CalcFromPosVel(const Vector r, const Vector v);

        # For small eccentricities a good approximation of true anomaly can be
        # obtained by the following formula (the error is of the order e^3)
        double EstimateTrueAnomaly(const double meanAnomaly) const
        double CalcEccentricAnomaly(const double meanAnomaly) const
        double SpeedAtDistance(const double distance) const
        void CalcTrueAnomaly(const double eccentricAnomaly)
        void Step(const double time)
        Orbit Predict(const double time) const


@cy.final
cdef class PyOrbit:
    cdef Orbit* _orbit
    cdef bint owning

    @staticmethod
    cdef inline PyOrbit wrap(Orbit *orbit):
        return PyOrbit(ptr=<long long>orbit)

    @staticmethod
    cdef inline PyOrbit cp(const Orbit &orbit):
        cdef Orbit *new_orbit = new Orbit(orbit)
        wrapper = PyOrbit(ptr=<long long>new_orbit)
        wrapper.owning = True
        return wrapper

    cdef inline Orbit* get(self):
        return self._orbit

    cpdef PyOrbit predict(self, double t)
