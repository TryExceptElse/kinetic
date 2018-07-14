from .vector import PyVector


class PyOrbit:
    def __init__(
            self,
            u: float = 0,
            a: float = 0,
            e: float = 0,
            i: float = 0,
            l: float = 0,
            w: float = 0,
            t: float = 0,
            r: PyVector = None,
            v: PyVector = None,
            **kwargs
    ) -> None: ...
    
    gravitational_parameter = ...  # type: float
    periapsis = ...  # type: float
    apoapsis = ...  # type: float
    eccentricity = ...  # type: float
    inclination = ...  # type: float
    longitude_of_ascending_node = ...  # type: float
    argument_of_periapsis = ...  # type: float
    true_anomaly = ...  # type: float
    semiparameter = ...  # type: float
    semi_major_axis = ...  # type: float
    semi_minor_axis = ...  # type: float
    period = ...  # type: float
    eccentric_anomaly = ...  # type: float
    mean_anomaly = ...  # type: float
    mean_motion = ...  # type: float
    time_since_periapsis = ...  # type: float
    min_speed = ...  # type: float
    max_speed = ...  # type: float
    position = ...  # type: PyVector
    velocity = ...  # type: PyVector
