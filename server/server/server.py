import asyncio
import typing as ty
import websockets
import logging

from server import event as e
from server.model.model import GameModel


class Server:
    """
    Main server class that controls socket instance
    """
    def __init__(self):
        self.model = GameModel()

    def start(self, host: ty.Any='localhost', port: int=8240):
        asyncio.get_event_loop().run_until_complete(
            websockets.serve(self.handle_msg, host, port))
        asyncio.get_event_loop().run_forever()

    async def handle_msg(self, msg: e.GameMsg):
        logger = logging.getLogger(__name__)
        logger.debug(f'Handling msg: {e}')
