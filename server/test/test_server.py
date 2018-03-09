
from unittest import TestCase

import asyncio
import websockets
import threading as th
import time

from server.server import Server
from server import event as e


class TestServer(TestCase):

    def test_server_can_be_instantiated(self):
        Server()

    def test_msg_handlers_are_annotated_and_found_correctly(self):
        server = Server()
        self.assertEqual(server.handlers[e.TestMsg], server.handle_test_msg)

    def test_server_can_register_connection(self):
        server = Server()
        connection_msg = e.ConnectRequestMsg('test_name')

        uri = 'ws://localhost:8765'
        server_thread = th.Thread(
            target=lambda: server.start('localhost', 8765),
            daemon=True)
        try:
            server_thread.start()
            time.sleep(0.01)  # give server thread a little time to start
            send_test_msg(connection_msg, uri)
            self.assertEqual(1, len(server.connections))
        finally:
            server.end()
            server_thread.join()

    def test_server_will_reject_repeated_socket_registrations(self):
        """
        Server should ignore second and later ConnectRequestMsg sent
        by a connected socket.
        """
        server = Server()
        connection_msg_a = e.ConnectRequestMsg('test_name')
        connection_msg_b = e.ConnectRequestMsg('test_name')

        uri = 'ws://localhost:8765'
        server_thread = th.Thread(
            target=lambda: server.start('localhost', 8765),
            daemon=True)
        try:
            server_thread.start()
            time.sleep(0.01)  # give server thread a little time to start

            async def test_send_duplicates():
                async with websockets.connect(uri) as web_socket:
                    await web_socket.send(e.encode_msg(connection_msg_a))
                    await web_socket.send(e.encode_msg(connection_msg_b))

            asyncio.get_event_loop().run_until_complete(test_send_duplicates())

            self.assertEqual(1, len(server.connections))
        finally:
            server.end()
            server_thread.join()


def send_test_msg(msg: e.GameMsg, uri):
    """
    Sends passed message to passed uri.
    Intended to be used for testing effect of sending a message
    to a server.
    :param msg: GameMsg
    :param uri: uri str
    :return: None
    """
    async def test_send_():
        async with websockets.connect(uri) as web_socket:
            await web_socket.send(e.encode_msg(msg))

    asyncio.get_event_loop().run_until_complete(test_send_())
