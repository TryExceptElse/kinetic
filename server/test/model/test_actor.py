from unittest import TestCase

from server.model.actor import PyActor


class TestBody(TestCase):
    def test_actor_can_be_initialized(self):
        actor = PyActor()
        self.assertNotEqual('', actor.id)
