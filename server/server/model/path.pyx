"""
This module defines wrappers and associated functions for the
functions, structs, and classes of actor/src/path.h.
"""

from enum import Enum


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
    def v(self) -> double:
        return PyVector.cp(self._data.v)


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
        if isinstance(direction, str):
            maneuver_type = self.Types[direction].value
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
