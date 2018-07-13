cimport libcpp.memory as mem
from libcpp.unordered_map cimport unordered_map
from libcpp.string cimport string

from server.model.actor cimport Actor, PyActor
from server.model.system cimport System, PySystem


cdef extern from "universe.h" namespace "kin" nogil:
    cppclass Universe:
        unordered_map[string, System].iterator FindSystem(const string id)
        unordered_map[string, Actor].iterator FindActor(const string id)

        # Getters
        const unordered_map[string, mem.unique_ptr[System]]& systems() const
        const unordered_map[string, mem.unique_ptr[Actor]]& actors() const
    
    
cdef extern from "universe.h" nogil:
    bint kin_Universe_AddSystem(Universe *uni, System *system)
    bint kin_Universe_AddActor(Universe *uni, Actor *actor)


cdef class PyUniverse:
    cdef Universe* _universe
    cdef bint owning

    @staticmethod
    cdef inline wrap(Universe* universe):
        return PyUniverse(ptr=<long long>universe)
    
    cdef inline Universe* get(self):
        return self._universe
        
    # Methods
    
    cpdef bint add_system(self, PySystem system)
    cpdef bint add_actor(self, PyActor actor)

    cpdef PySystem get_system(self, str id)
    cpdef PyActor get_actor(self, str id)
