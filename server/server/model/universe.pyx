from libcpp.utility cimport pair
from cython.operator cimport dereference as deref


cdef class PyUniverse:
    def __cinit__(self, **kwargs):
        if 'ptr' in kwargs:
            self._universe = <Universe *><long long>kwargs['ptr']
            self.owning = False
            return
        self._universe = new Universe()
        self.owning = True

    def __dealloc__(self):
        if self.owning:
            del self._universe

    cpdef bint add_system(self, PySystem system):
        if system.owning:
            system.owning = False
        else:
            raise ValueError('Universe.AddSystem() gives universe ownership '
                             f'of system. Passed PySystem {system} did not '
                             'have ownership, so ownership could not be passed')
        return kin_Universe_AddSystem(self.get(), system.get())

    cpdef bint add_actor(self, PyActor actor):
        if actor.owning:
            actor.owning = False
        else:
            raise ValueError('Universe.AddActor() gives universe ownership '
                             f'of actor. Passed PyActor {actor} did not '
                             'have ownership, so ownership could not be passed')
        return kin_Universe_AddActor(self.get(), actor.get())

    cpdef PySystem get_system(self, str id):
        cdef unordered_map[string, mem.unique_ptr[System]].iterator it = \
            self._universe.FindSystem(id)
        if it == self._universe.systems().end():
            raise KeyError(f'System id not found: {id.encode("ascii")}')
        return PySystem.wrap(<System *><long long>deref(it).second.get())

    cpdef PyActor get_actor(self, str id):
        cdef unordered_map[string, mem.unique_ptr[Actor]].iterator it = \
            self._universe.FindActor(id.encode('utf-8'))
        if it == self._universe.actors().end():
            raise KeyError(f'Actor id not found: {repr(id)}')
        return PyActor.wrap(<Actor *><long long>deref(it).second.get())
