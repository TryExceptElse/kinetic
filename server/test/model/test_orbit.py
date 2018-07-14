from unittest import TestCase

from server.model.orbit import PyOrbit
from server.model.vector import PyVector
import server.model.const as const


class TestOrbit(TestCase):
    def test_eccentricity_is_calculated_correct(self):
        """
        This test mirrors the one of the same name in testorbit.cc
        """
        r = PyVector(617244712358.0, -431694791368.0, -12036457087.0)
        v = PyVector(7320.0, 11329.0, -0211.0)
        orbit = PyOrbit(u=const.G * 1.98891691172467e30, r=r, v=v)

        self.assertAlmostEqual(0.049051434386, orbit.eccentricity, 4)
