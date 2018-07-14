from enum import Enum
import typing as ty

from .system import PySystem
from .vector import PyVector
from .orbit import PyOrbit
from .body import PyBody


class PyPerformanceData:
    def __init__(self, ve: float, thrust: float) -> None: ...
    ve = ...  # type: float
    thrust = ...  # type: float
    flow_rate = ...  # type: float


class PyKinematicData:
    def __init__(self, r: PyVector, v: PyVector) -> None: ...
    r = ...  # type: PyVector
    v = ...  # type: PyVector


class PyOrbitData:
    def __init__(self, **kwargs) -> None: ...
    orbit = ...  # type: PyOrbit
    body = ...  # type: PyBody



class ManeuverTypes(Enum): ...

class PyManeuver:

    Types = ManeuverTypes

    def __init__(
            self,
            direction: ty.Union[str, PyVector, int],
            dv: float = 0.0,
            performance_data: PyPerformanceData = None,
            m0: float = 0.0,
            t0: float = 0.0
    ) -> None: ...

    type = ...  # type: str
    dv = ...  # type: float
    m0 = ...  # type: float
    m1 = ...  # type: float
    t0 = ...  # type: float
    t1 = ...  # type: float
    duration = ...  # type: float
    mass_fraction = ...  # type: float
    expened_mass = ...  # type: float
    def find_mass_at_time(self, t: float): ...


class PyFlightPath:
    def __init__(
            self,
            system: PySystem,
            r: PyVector,
            v: PyVector,
            t: float,
            **kwargs
    ) -> None: ...

    def predict(self, time: float) -> PyKinematicData: ...

    def predict_orbit(self, time: float, body: PyBody = None) -> PyOrbitData:...

    def find_maneuver(self, t: float) -> PyManeuver: ...

    def find_next_maneuver(self, t: float) -> PyManeuver: ...

    def add(self, maneuver: PyManeuver) -> None: ...

    def clear(self) -> bool: ...

    def clear_after(self, t: float) -> bool: ...

    def remove(self, maneuver: PyManeuver) -> bool: ...