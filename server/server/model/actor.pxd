from .vector cimport Vector, PyVector
from libcpp.memory cimport shared_ptr, make_shared

cdef extern from "actor.h" namespace "kin" nogil:
    cdef cppclass Actor:
        Vector local_position() const;
        Vector local_velocity() const;
        Vector world_position() const;
        Vector world_velocity() const;


cdef class PyActor:
    cdef shared_ptr[Actor] _actor
