cimport cython as cy


cdef extern from "vector.h" namespace "kin" nogil:
    cdef cppclass Vector:
        Vector()
        Vector(double x, double y, double z)
        double norm() const
        double squaredNorm() const

        double x() const
        double y() const
        double z() const


@cy.final
cdef class PyVector:
    cdef Vector _vector

    @staticmethod
    cdef PyVector cp(Vector vector)

    cdef inline Vector val(self)
