from .vector cimport Vector, PyVector


cdef class PyActor:
    """
    Wraps Actor cpp class
    """

    def __cinit__(self, **kwargs) -> None:
        if 'ptr' in kwargs:
            self._actor = <Actor *><long long>kwargs.get('ptr')
            self.owning = False
            return
        self.owning = True
        # TODO: support additional construction methods.
        # TODO: store reference to system.
        self._actor = new Actor()

    def __dealloc__(self) -> None:
        if self.owning:
            del self._actor
            self._actor = NULL

    @staticmethod
    cdef PyActor wrap(Actor* actor):
        return PyActor(ptr=<long long>actor)

    cdef Actor* get(self):
        return self._actor

#    cpdef PyKinematicData predict(self, double t):
#        return PyKinematicData.cp(self._actor.Predict(t))
