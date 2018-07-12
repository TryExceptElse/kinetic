

cdef class PySystem:
    def __cinit__(self, id: str = '', root: PyBody = None, **kwargs):
        self.universe = None
        self._id = id
        if 'ptr' in kwargs:
            self._system = <System *><long long>kwargs.get('ptr')
            self.owning = False
            return
        # If a new system is to be constructed, a root body must have
        # been passed.
        if not root:
            raise TypeError(f'Root must be passed to a new System. Got: {root}')
        # If root already has an owner, something may have gone wrong.
        if root.system:
            raise Warning(f'Passed root already has system: {root.system}')
        root.system = self  # Ensure proper deletion order by storing reference.
        root.owning = False  # System now takes ownership of root.
        if id:
            self._system = kin_NewSystemFromRoot(root.get())
        else:
            self._system = kin_NewSystemFromIdAndRoot(
                id.encode('utf-8'), root.get())
        self.owning = True

    def __dealloc__(self) -> None:
        if self.owning:
            del self._system
            self._system = NULL

    @staticmethod
    cdef PySystem wrap(System* system):
        return PySystem(<long long>system)

    cdef System* get(self):
        return self._system

    # General methods

    cpdef void add_actor(self, PyActor actor):
        self._system.AddActor(actor.get())

    # Getters

    @property
    def id(self) -> str:
        return self._system.id()

    @property
    def root(self) -> PyBody:
        return PyBody.wrap(&self._system.root())
