cimport libcpp.memory as mem
from libcpp.string cimport string

from .vector cimport Vector, PyVector
from .path cimport KinematicData, PyKinematicData


cdef extern from "actor.h" namespace "kin" nogil:
    cdef cppclass Actor:
        Actor()
        Actor(const Vector r, const Vector v)

        KinematicData Predict(double t) const

        # Getters
        const string& id() const


cdef class PyActor:
    cdef Actor* _actor
    cdef bint owning

    @staticmethod
    cdef PyActor wrap(Actor* actor)

    cdef Actor* get(self)

    cpdef PyKinematicData predict(self, double t)
