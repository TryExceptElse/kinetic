from .vector cimport wrap_vector


cdef class PyActor:
    """
    Wraps Actor cpp class
    """

    def __init__(self, new=True):
        if new:
            self._actor = make_shared[Actor]()

    @property
    def local_position(self):
        return wrap_vector(self._actor.get().local_position())

    @property
    def local_velocity(self):
        return wrap_vector(self._actor.get().local_velocity())

    @property
    def world_position(self):
        return wrap_vector(self._actor.get().world_position())

    @property
    def world_velocity(self):
        return wrap_vector(self._actor.get().world_velocity())
