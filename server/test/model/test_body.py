from unittest import TestCase

from server.model.body import PyBody


class TestBody(TestCase):
    def test_body_can_be_initialized(self):
        body = PyBody(gm=10000, r=1000)
        self.assertNotEqual('', body.id)
        self.assertEqual(10000, body.gm)
        self.assertEqual(1000, body.radius)
        pass
