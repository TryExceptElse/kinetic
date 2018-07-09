from unittest import TestCase

import server.model.path as path


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
