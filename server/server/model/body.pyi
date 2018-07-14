from .orbit import PyOrbit
from .vector import PyVector


class PyBody:
    def __init__(
            self,
            id: str = '',
            gm: float = 0,
            r: float = 0,
            parent: PyBody = None,
            orbit: PyOrbit = None,
            **kwargs
    ) -> None: ...
    
    # Methods
    
    def is_parent_of(self, other: PyBody) -> bool: ...
    def predict_local_position(self, t: float) -> PyVector: ...
    def predict_system_position(self, t: float) -> PyVector: ...
    def predict_local_velocity(self, t: float) -> PyVector: ...
    def predict_system_velocity(self, t: float) -> PyVector: ...
    
    # Properties

    has_parent = ...  # type: bool
    id = ...  # type: str
    parent = ...  # type: PyBody
    gm = ...  # type: float
    radius = ...  # type: float
