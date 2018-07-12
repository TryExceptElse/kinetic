#cython: c_string_encoding='ascii'

from libc.stdio cimport printf


cdef class PyBody:
    def __cinit__(
            self,
            str id = '',
            double gm = 0,
            double r = 0,
            PyBody parent = None,
            PyOrbit orbit = None,
            **kwargs
    ) -> None:
        cdef Body* c_parent
        cdef Orbit* c_orbit
        self._str_id = ''
        self.system = None
        if 'ptr' in kwargs:
            self._body = <Body *><long long>kwargs['ptr']
            self.owning = False
            return
        if gm <= 0:
            raise ValueError(f'GM must be > 0. Got: {gm}')
        if r <= 0:
            raise ValueError(f'Radius must be > 0. Got: {r}')
        if (parent is None) != (orbit is None):
            raise ValueError(f'parent and orbit should either be both passed'
                    f'or not passed at all. Got: {parent} and {orbit}')

        if parent:
            c_parent = parent.get()
            c_orbit = orbit.get()
        else:
            c_parent = NULL
            c_orbit = NULL
        self._body = new Body(id, gm, r, c_parent, c_orbit)
        self.owning = True
#       Body(const std::string id, const double GM, const double r,
#               Body *parent = nullptr, Orbit *orbit = nullptr)

    def __dealloc__(self):
        if self.owning:
            del self._body
            self._body = NULL

    # Methods

    cpdef bool is_parent_of(self, PyBody other):
        return self._body.IsParent(other.get()[0])

    cpdef PyVector predict_local_position(self, double t):
        return PyVector.cp(self._body.PredictLocalPosition(t))

    cpdef PyVector predict_system_position(self, double t):
        return PyVector.cp(self._body.PredictSystemPosition(t))

    cpdef PyVector predict_local_velocity(self, double t):
        return PyVector.cp(self._body.PredictLocalVelocity(t))

    cpdef PyVector predict_system_velocity(self, double t):
        return PyVector.cp(self._body.PredictSystemVelocity(t))

    # Properties

    @property
    def has_parent(self) -> bool:
        return self._body.HasParent()

    @property
    def id(self) -> str:
        if self._str_id == '':
            self._str_id = self._body.id()
        return self._str_id

    @property
    def parent(self) -> PyBody:
        return PyBody(<long long>self._body.parent())

    @property
    def gm(self) -> double:
        return self._body.gm()

    @property
    def radius(self) -> double:
        return self._body.radius()
