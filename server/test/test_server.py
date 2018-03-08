
from unittest import TestCase

from server.server import Server
from server.event import TestMsg


class TestServer(TestCase):

    def test_server_can_be_instantiated(self):
        server = Server()

    def test_msg_handlers_are_annotated_and_found_correctly(self):
        server = Server()
        self.assertEqual(server.handlers[TestMsg], server.handle_test_msg)
