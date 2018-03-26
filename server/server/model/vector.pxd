

cdef extern from "vector.h" namespace "kin" nogil:
    cdef cppclass Vector:
        double x, y, z

        Vector()
        Vector(double x, double y, double z)
        double len() const
        double sqlen() const


cdef class PyVector:
    cdef Vector _vector

    cdef inline Vector get_vector(self)
    cpdef double sqlen(self) except -1.0
    cpdef double len(self) except -1.0

cdef PyVector wrap_vector(Vector vector)
