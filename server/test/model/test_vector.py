from unittest import TestCase

from server.model.vector import PyVector


class TestVector(TestCase):
    def test_vector_squared_length_is_calculated(self):
        v = PyVector(1, 2, 3)
        self.assertEqual(14, v.sqlen())
