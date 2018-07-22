

cdef class PyVector:
    """ Wraps Vector """
    def __init__(self, x=0, y=0, z=0):
        self._vector = Vector(x, y, z)

    @staticmethod
    cdef PyVector cp(Vector vector):
        cdef PyVector wrapper = PyVector()
        wrapper._vector = vector
        return wrapper

    cdef inline Vector val(self):
        return self._vector

    @property
    def squared_norm(self) -> double:
        return self._vector.squaredNorm()

    @property
    def norm(self) -> double:
        return self._vector.norm()

    @property
    def x(self) -> float:
        return self._vector.x()

    @property
    def y(self) -> float:
        return self._vector.y()

    @property
    def z(self):
        return self._vector.z()

    def __str__(self) -> str:
        return f'Vector[x={self.x}, y={self.y}, z={self.z}]'
