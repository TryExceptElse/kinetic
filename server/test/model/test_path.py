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
