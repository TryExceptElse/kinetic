cimport libcpp.memory as mem
from libcpp.string cimport string

from server.model.body cimport Body, PyBody
from server.model.vector cimport Vector, PyVector
from server.model.actor cimport Actor, PyActor


cdef extern from "system.h" namespace "kin" nogil:
    cppclass System:
        System(mem.unique_ptr[Body] root)
        System(string id, mem.unique_ptr[Body] root)

        # Find body in System which is the primary influence at
        # passed position vector.
        const Body& FindPrimaryInfluence(const Vector r, double t) const;
        void AddActor(Actor *actor);

        # getters
        Vector v() const
        Body& root() const
        const string id() const


cdef extern from "system.h" nogil:
    System* kin_NewSystemFromRoot(Body *root)
    System* kin_NewSystemFromIdAndRoot(char *id, Body *root)


cdef class PySystem:
    cdef System* _system
    cdef bint owning
    cdef str _id
    cdef object universe  # PyUniverse universe

    @staticmethod
    cdef PySystem wrap(System *system)

    cdef System* get(self)

    cpdef void add_actor(self, PyActor actor)
