from unittest import TestCase

from server.model.universe import PyUniverse
from server.model.actor import PyActor


class TestUniverse(TestCase):
    def test_universe_can_be_initialized(self):
        PyUniverse()

    def test_actor_can_be_added_to_universe_and_retrieved(self):
        actor_a = PyActor()
        id_ = actor_a.id
        uni = PyUniverse()
        uni.add_actor(actor_a)

        actor_b = uni.get_actor(id_)
        self.assertEqual(id_, actor_b.id)
