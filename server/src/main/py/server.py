import websockets
import asyncio

from . import event as e


class Server:
    """
    Main server class that controls socket instance
    """
    def __init__(self):
        self.socket = websockets.Socket

    def start(self):
        asyncio.get_event_loop().run_until_complete(
            websockets.serve(self.handle_msg, 'localhost', 8240))
        asyncio.get_event_loop().run_forever()

    async def handle_msg(self, msg: e.GameMsg):
        
