"""
This module defines wrappers and associated functions for the
functions, structs, and classes of actor/src/path.h.
"""


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
        return PyVector.cp(self._data.r())

    @property
    def v(self) -> double:
        return PyVector.cp(self._data.v())
