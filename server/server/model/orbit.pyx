
cdef class PyOrbit:
    def __cinit__(
            self,
            u: double = 0,
            a: double = 0,
            e: double = 0,
            i: double = 0,
            l: double = 0,
            w: double = 0,
            t: double = 0,
            r: PyVector = None,
            v: PyVector = None,
            **kwargs
    ) -> None:
        if 'ptr' in kwargs:
            self._orbit = <Orbit *><long long>kwargs['ptr']
            self.owning = False
        else:
            if r is None:
                self._orbit = new Orbit(u, a, e, i, l, w, t)
            else:
                self._orbit = new Orbit(u, r.val(), v.val())
            self.owning = True

    def __dealloc__(self):
        if self.owning:
            del self._orbit

    @property
    def gravitational_parameter(self) -> double:
        return self._orbit.gravitational_parameter()

    @property
    def semi_major_axis(self) -> double:
        return self._orbit.semi_major_axis()

    @property
    def periapsis(self) -> double:
        return self._orbit.periapsis()

    @property
    def apoapsis(self) -> double:
        return self._orbit.apoapsis()

    @property
    def eccentricity(self) -> double:
        return self._orbit.eccentricity()

    @property
    def inclination(self) -> double:
        return self._orbit.inclination()

    @property
    def longitude_of_ascending_node(self) -> double:
        return self._orbit.longitude_of_ascending_node()

    @property
    def argument_of_periapsis(self) -> double:
        return self._orbit.argument_of_periapsis()

    @property
    def true_anomaly(self) -> double:
        return self._orbit.true_anomaly()

    @property
    def semiparameter(self) -> double:
        return self._orbit.semiparameter()

    @property
    def semi_minor_axis(self) -> double:
        return self._orbit.semi_minor_axis()

    @property
    def period(self) -> double:
        return self._orbit.period()

    @property
    def eccentric_anomaly(self) -> double:
        return self._orbit.eccentric_anomaly()

    @property
    def mean_anomaly(self) -> double:
        return self._orbit.mean_anomaly()

    @property
    def mean_motion(self) -> double:
        return self._orbit.mean_motion()

    @property
    def time_since_periapsis(self) -> double:
        return self._orbit.time_since_periapsis()

    @property
    def min_speed(self) -> double:
        return self._orbit.min_speed()

    @property
    def max_speed(self) -> double:
        return self._orbit.max_speed()

    @property
    def position(self) -> PyVector:
        return PyVector.cp(self._orbit.position())

    @property
    def velocity(self) -> PyVector:
        return PyVector.cp(self._orbit.velocity())
