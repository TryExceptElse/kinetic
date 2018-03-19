

cdef class PyVector:
    """ Wraps Vector """
    def __init__(self):
        pass

    cdef inline Vector get_vector(self):
        return self._vector

    cpdef double sqlen(self) except -1.0:
        return self._vector.sqlen()

    cpdef double len(self) except -1.0:
        return self._vector.len()


cdef PyVector wrap_vector(Vector vector):
    cdef PyVector wrapper = PyVector
    wrapper._vector = vector
