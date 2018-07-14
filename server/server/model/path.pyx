"""
This module defines wrappers and associated functions for the
functions, structs, and classes of actor/src/path.h.
"""

from enum import Enum
from libcpp.cast cimport const_cast


cdef class PyPerformanceData:
    def __init__(self, ve: double, thrust: double) -> None:
        self._data = mem.make_unique[PerformanceData](ve, thrust)

    cdef inline PerformanceData* get(self):
        return self._data.get()

    @staticmethod
    cdef PyPerformanceData cp(data: PerformanceData):
        return PyPerformanceData(data.ve(), data.thrust())

    @property
    def ve(self) -> double:
        return self._data.get().ve()

    @property
    def thrust(self) -> double:
        return self._data.get().thrust()

    @property
    def flow_rate(self) -> double:
        return self._data.get().flow_rate()


cdef class PyKinematicData:
    def __init__(self, r: PyVector, v: PyVector) -> None:
        self._data = KinematicData(r.val(), v.val())

    cdef inline KinematicData val(self):
        return self._data

    @staticmethod
    cdef PyKinematicData cp(KinematicData data):
        # Annoyingly roundabout. More direct method?
        return PyKinematicData(PyVector.cp(data.r), PyVector.cp(data.v))

    @property
    def r(self) -> PyVector:
        return PyVector.cp(self._data.r)

    @property
    def v(self) -> PyVector:
        return PyVector.cp(self._data.v)


cdef class PyOrbitData:
    def __init__(self, **kwargs):
        if 'ptr' in kwargs:
            self._data = mem.make_unique[OrbitData](
                    (<OrbitData *><long long>kwargs['ptr'])[0])
        else:
            raise TypeError('Expected ptr to orbit data to copy.')
        self._orbit = None
        self._body = None

    @staticmethod
    cdef PyOrbitData cp(const OrbitData &data):
        return PyOrbitData(ptr=<long long>&data)

    @property
    def orbit(self) -> PyOrbit:
        if self._orbit is None:
            self._orbit = PyOrbit.cp(self._data.get().orbit())
        return self._orbit

    @property
    def body(self) -> PyBody:
        if self._body is None:
            # c++ OrbitData has const body reference, because it is not
            # Expected to be changed, however since it becomes very
            # inconvenient to wrap a const object, we simply
            # cast it away here.
            self._body = PyBody.wrap(<Body *>(&self._data.get().body()))
        return self._body


class ManeuverTypes(Enum):
    Prograde = ManeuverType.kPrograde
    Retrograde = ManeuverType.kRetrograde
    Normal = ManeuverType.kNormal
    AntiNormal = ManeuverType.kAntiNormal
    Radial = ManeuverType.kRadial
    AntiRadial = ManeuverType.kAntiRadial
    Fixed = ManeuverType.kFixed


cdef class PyManeuver:

    Types = ManeuverTypes

    def __cinit__(
            self,
            direction,
            dv: double = 0.0,
            performance_data: PyPerformanceData = None,
            m0: double = 0.0,
            t0: double = 0.0
    ) -> None:
        cdef Maneuver.ManeuverType maneuver_type
        cdef PyVector py_vector
        cdef Vector vector
        cdef PerformanceData* data = performance_data.get()
        cdef Maneuver* ptr
        if isinstance(direction, (str, ManeuverTypes)):
            if (isinstance(direction, str)):
                maneuver_type = self.Types[direction].value
            else:
                maneuver_type = direction.value
            self._maneuver = mem.make_unique[Maneuver](
                    maneuver_type, dv, data[0], m0, t0)
        elif isinstance(direction, PyVector):
            py_vector = direction
            vector = py_vector.val()
            self._maneuver = mem.make_unique[Maneuver](
                    vector, dv, data[0], m0, t0)
        elif isinstance(direction, int):  # check if first arg is python int
            ptr = <Maneuver *><long long>direction  # if so cast to pointer
            self._maneuver = mem.make_unique[Maneuver](ptr[0])
        else:
            raise TypeError(f'Unexpected type of first argument: {direction}'
                            f'(type: {type(direction)})')

    @staticmethod
    cdef PyManeuver cp(Maneuver maneuver):
        pass

    cdef inline Maneuver* get(self):
        return self._maneuver.get()

    @property
    def type(self) -> unicode:
        return self.Types(self.get().type()).name

    @property
    def dv(self) -> double:
        return self.get().dv()

    @property
    def m0(self) -> double:
        return self.get().m0()

    @property
    def m1(self) -> double:
        return self.get().m1()

    @property
    def t0(self) -> double:
        return self.get().t0()

    @property
    def t1(self) -> double:
        return self.get().t1()

    @property
    def duration(self) -> double:
        return self.get().duration()

    @property
    def mass_fraction(self) -> double:
        return self.get().mass_fraction()

    @property
    def expended_mass(self) -> double:
        return self.get().expended_mass()

    cpdef double find_mass_at_time(self, double t):
        return self.get().FindMassAtTime(t)


cdef class PyFlightPath:
    def __cinit__(
            self,
            system: PySystem,
            r: PyVector,
            v: PyVector,
            t: double,
            **kwargs
    ) -> None:
        if 'ptr' in kwargs:
            self._path = <FlightPath *><long long>kwargs['ptr']
            self.owning = False
            self._system = None
            return
        self._path = new FlightPath(system.get()[0], r.val(), v.val(), t)
        self.owning = True
        self._system = system

    cpdef PyKinematicData predict(self, double time):
        return PyKinematicData.cp(self._path.Predict(time))

    cpdef PyOrbitData predict_orbit(self, double time, PyBody body = None):
        if body:
            return PyOrbitData.cp(self._path.PredictOrbit(time, body.get()))
        else:
            return PyOrbitData.cp(self._path.PredictOrbit(time))

    cpdef PyManeuver find_maneuver(self, double t):
        return PyManeuver.cp(self._path.FindManeuver(t)[0])

    cpdef PyManeuver find_next_maneuver(self, double t):
        return PyManeuver.cp(self._path.FindNextManeuver(t)[0])

    cpdef void add(self, PyManeuver maneuver) except *:
        if maneuver.get() == NULL:
            raise ValueError(
            f'Passed maneuver {maneuver} had null wrapped address')
        self._path.Add(maneuver.get()[0])

    cpdef bint clear(self):
        return self._path.Clear()

    cpdef bint clear_after(self, double t):
        return self._path.ClearAfter(t)

    cpdef bint remove(self, PyManeuver maneuver):
        return self._path.Remove(maneuver.get()[0])
