from unittest import TestCase

from server.model.system import PySystem
from server.model.body import PyBody


class TestSystem(TestCase):
    def test_system_can_be_initialized(self):
        root_body = PyBody(gm=10_000, r=1000)
        system = PySystem(root=root_body)
        self.assertEqual(1000, system.root.radius)
