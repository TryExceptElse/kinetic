
from unittest import TestCase

from server.server import Server


class TestServer(TestCase):

    def test_server_can_be_instantiated(self):
        server = Server()
