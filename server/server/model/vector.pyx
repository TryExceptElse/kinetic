

cdef class PyVector:
    """ Wraps Vector """
    def __init__(self, x=0, y=0, z=0):
        self._vector.x = x
        self._vector.y = y
        self._vector.z = z

    @staticmethod
    cdef PyVector cp(Vector vector):
        cdef PyVector wrapper = PyVector
        wrapper._vector = vector
        return wrapper

    cdef inline Vector val(self):
        return self._vector

    cpdef double sqlen(self) except -1.0:
        return self._vector.sqlen()

    cpdef double len(self) except -1.0:
        return self._vector.len()

    @property
    def x(self) -> float:
        return self._vector.x

    @x.setter
    def x(self, value: float):
        self._vector.x = value

    @property
    def y(self) -> float:
        return self._vector.y

    @y.setter
    def y(self, value: float):
        self._vector.y = value

    @property
    def z(self):
        return self._vector.z

    @z.setter
    def z(self, value):
        self._vector.z = value
