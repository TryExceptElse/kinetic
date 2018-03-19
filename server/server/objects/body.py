from .gameobject import GameObject
from ..util import const


class Body(GameObject):
    """
    A Body is an object with sufficient mass to have practical
    gravitational effects, and a sphere of influence.
    A Body can be any large object, such as a large asteroid or star,
    and is subclassed by Spheroids (planets, planetoids, and moons)
    stars, and similar.
    """

    def __init__(self, uuid: str='', name: str='', mass=1, radius=1):
        super().__init__(uuid, name)
        self.mass: float = mass
        self.radius: float = radius

    @property
    def mu(self):
        """

        :return:
        """
        return const.G * self.mass
