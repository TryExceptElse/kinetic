from .actor import PyActor
from .body import PyBody


class PySystem:
    def __init__(self, id: str = '', root: PyBody = None, **kwargs): ...

    # General methods

    def add_actor(self, actor: PyActor) -> None: ...

    # Getters

    id = ...  # type: str
    root = ...  # type: PyBody
