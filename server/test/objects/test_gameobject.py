from unittest import TestCase

from server.objects.gameobject import GameObject


class TestGameObject(TestCase):
    def test_object_is_given_unique_id(self):
        a = GameObject()
        b = GameObject()

        self.assertNotEqual(a.id, b.id)

    def test_object_can_be_passed_id_at_initialization(self):
        uuid = '1234567890'
        go = GameObject(uuid=uuid)
        self.assertEqual('1234567890', go.id)
