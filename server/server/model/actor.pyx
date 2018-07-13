from .vector cimport Vector, PyVector


cdef class PyActor:
    """
    Wraps Actor cpp class
    """

    def __cinit__(
            self,
            actor_type: str = '',
            id: str = '',
            r: PyVector = None,
            v: PyVector = None,
            **kwargs
    ) -> None:
        if 'ptr' in kwargs:
            self._actor = <Actor *><long long>kwargs.get('ptr')
            self.owning = False
            return
        self.owning = True
        # TODO: support additional construction methods.
        # TODO: store reference to system.
        self._actor = new Actor(
                actor_type.encode('utf-8'),
                id.encode('utf-8'),
                r.val() if r else Vector(),
                v.val() if v else Vector())

    def __dealloc__(self) -> None:
        if self.owning:
            del self._actor
            self._actor = NULL

    @staticmethod
    cdef PyActor wrap(Actor* actor):
        return PyActor(ptr=<long long>actor)

    cdef Actor* get(self):
        return self._actor

    cpdef PyKinematicData predict(self, double t):
        return PyKinematicData.cp(self._actor.Predict(t))

    # Getters

    @property
    def id(self) -> str:
        return self._actor.id().decode('utf-8')

    @property
    def actor_type(self) -> str:
        return self._actor.actor_type().decode('utf-8')
