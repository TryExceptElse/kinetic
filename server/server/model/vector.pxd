

cdef extern from "vector.h" namespace "kin" nogil:
    cdef cppclass Vector:
        double x, y, z

        Vector()
        Vector(double x, double y, double z)
        double len() const
        double sqlen() const


cdef class PyVector:
    cdef Vector _vector

    @staticmethod
    cdef PyVector cp(Vector vector)

    cdef inline Vector val(self)
