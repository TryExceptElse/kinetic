from unittest import TestCase

import server.model.const as const
import server.model.path as path
from server.model.system import PySystem
from server.model.body import PyBody
from server.model.vector import PyVector


class TestPerformanceData(TestCase):
    def test_performance_data_flow_rate_is_calculated(self):
        performance_data = path.PyPerformanceData(1000, 100_000)
        self.assertEqual(100, performance_data.flow_rate)

    def test_performance_data_fields_are_accessible(self):
        data = path.PyPerformanceData(1000, 100_000)
        self.assertEqual(1000, data.ve)
        self.assertEqual(100_000, data.thrust)


class TestManeuver(TestCase):
    def test_maneuver_can_be_created_and_fields_initialized(self):
        data = path.PyPerformanceData(3000, 20_000)
        maneuver = path.PyManeuver("Prograde", 2000, data, 150, 10)
        self.assertEqual(2000, maneuver.dv)
        self.assertEqual("Prograde", maneuver.type)
        self.assertEqual(150, maneuver.m0)
        self.assertEqual(10, maneuver.t0)


class TestFlightPath(TestCase):
    def test_path_with_no_maneuvers_can_be_calculated(self):
        """ 
        This method is intended to be a direct translation of the 
        test in testpath.cc of the same name.
        """
        body = PyBody(gm=const.G * 1.98891691172467e30, r=10)
        system = PySystem(root=body)
        r = PyVector(617244712358.0, -431694791368.0, -12036457087.0)
        v = PyVector(7320.0, 11329.0, -0211.0)
        path_ = path.PyFlightPath(system, r, v, 0)
        prediction: path.PyKinematicData = path_.predict(374942509.78053558 / 2)
        
        position: PyVector = prediction.r
        self.assertAlmostEqual(-712305324741.15112, position.x, 0)
        self.assertAlmostEqual(365151451881.22858, position.y, 0)
        self.assertAlmostEqual(14442203602.998617, position.z, 0)
        
    def test_path_can_be_calculated_with_a_maneuver(self):
        """
        This method is intended to be a direct translation of the 
        test in testpath.cc of the same name.
        """
        body = PyBody(gm=const.G * 1.98891691172467e30, r=10)
        system = PySystem(root=body)
        r = PyVector(617244712358.0, -431694791368.0, -12036457087.0)
        v = PyVector(7320.0, 11329.0, -0211.0)
        path_ = path.PyFlightPath(system, r, v, 0)

        half_orbit_t = 374942509.78053558 / 2
        performance = path.PyPerformanceData(3000, 20000)  # ve, thrust
        path_.add(path.PyManeuver(
            path.PyManeuver.Types.Prograde,
            2000,
            performance,
            150,
            half_orbit_t
        ))

        # predict orbit of 1/2 period from t0.
        prediction1 = path_.predict(half_orbit_t)

        # This test doesn't try to determine a precise position, just that
        # the calculation can complete, and results in a changed orbit.
        position1 = prediction1.r
        self.assertAlmostEqual(-712305324741.15112, position1.x, 0)
        self.assertAlmostEqual(365151451881.22858, position1.y, 0)
        self.assertAlmostEqual(14442203602.998617, position1.z, 0)

        # predict orbit of 3/2 period from t0.
        # Orbit should have changed notably
        prediction2 = path_.predict(374942509.78053558 * 3.0 / 2.0)
        position2 = prediction2.r
        self.assertNotAlmostEqual(-712305324741.15112, position2.x, 0)
        self.assertNotAlmostEqual(365151451881.22858, position2.y, 0)
        self.assertNotAlmostEqual(14442203602.998617, position2.z, 0)

    def test_path_maneuver_increases_velocity(self):
        """
        This method is intended to be a direct translation of the
        test in testpath.cc of the same name.
        """
        body = PyBody(gm=const.G * 1.98891691172467e30, r=10)
        system = PySystem(root=body)
        r = PyVector(617244712358.0, -431694791368.0, -12036457087.0)
        v = PyVector(7320.0, 11329.0, -0211.0)
        path_ = path.PyFlightPath(system, r, v, 0)

        half_orbit_t = 374942509.78053558 / 2
        performance = path.PyPerformanceData(3000, 20000)  # ve, thrust
        dv = 2000
        maneuver = path.PyManeuver(
            path.PyManeuver.Types.Prograde,
            dv,
            performance,
            150,
            half_orbit_t
        )
        path_.add(maneuver)

        # Predict orbit of 1/2 period from t0.
        prediction0 = path_.predict(half_orbit_t)
        # Predict orbit at end of burn
        prediction1 = path_.predict(maneuver.t1)

        # This test doesn't try to determine a precise velocity, just that
        # it has increased a roughly expected amount.
        v0 = prediction0.v
        v1 = prediction1.v

        self.assertAlmostEqual(v1.len, v0.len + dv, -1)  # within ~10
