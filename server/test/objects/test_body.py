from unittest import TestCase

from server.objects.body import Body


class TestBody(TestCase):
    def test_body_mu_is_correct(self):
        body = Body(mass=10)
        self.assertEqual(6.674e-10, body.mu)
