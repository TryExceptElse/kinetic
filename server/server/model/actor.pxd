cimport libcpp.memory as mem
from libcpp.string cimport string

from .vector cimport Vector, PyVector
from .path cimport KinematicData, PyKinematicData
from .universe cimport Universe, PyUniverse


cdef extern from "actor.h" namespace "kin" nogil:
    cdef cppclass Actor:

        Actor(const string actor_type, const string id,
            const Vector r, const Vector v);
        Actor(const string &json);
        Actor(Universe *universe, const string actor_type,
            const string id, const Vector r, const Vector v)
        Actor(const Universe *universe, const string &json)

        KinematicData Predict(double t) const

        # Getters
        const string& id() const
        const string& actor_type() const


cdef class PyActor:
    cdef Actor* _actor
    cdef bint owning

    @staticmethod
    cdef PyActor wrap(Actor* actor)

    cdef Actor* get(self)

    cpdef PyKinematicData predict(self, double t)
