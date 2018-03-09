import asyncio
import typing as ty
import websockets
import logging

from uuid import uuid4

from server import event as e
from server.model.model import GameModel

# typedefs

HandlerDict = ty.Dict[ty.Type, ty.Callable[[type, e.GameMsg], ty.Any]]


def handler(msg_type: ty.Type):
    """
    Decorator for methods handling a GameMsg.

    Sets an attribute on the passed function that is checked for
    at Server class instantiation, and which is used to create a
    dictionary of handlers, which has GameMsg types as key, and
    the decorated methods as values.

    :param msg_type: GameMsg type
    :return: Callable
    """
    if not issubclass(msg_type, e.GameMsg):
        raise TypeError(
            f'Expected GameMsg class or subclass. Got: {msg_type}')

    def decorator(f):
        f.__handled_type = msg_type
        return f

    return decorator


def get_handled_type(f) -> ty.Type:
    return f.__handled_type


class Server:
    """
    Main server class that controls socket instance.
    """

    def __init__(self):
        self.model = GameModel()
        self.handlers: HandlerDict = self.find_handlers()
        self.connections = {}
        self._live = False
        self.event_loop: asyncio.AbstractEventLoop or None = None

    def find_handlers(self) -> \
            ty.Dict[ty.Type, ty.Callable[[e.GameMsg], ty.Any]]:
        """
        Finds handlers among own attributes
        :return: dictionary of handlers, with key as handled type.
        """
        handlers = {}
        for attr in [getattr(self, attr_name) for attr_name in dir(self)]:
            try:
                handled_type: ty.Type = get_handled_type(attr)
            except AttributeError:
                pass
            else:
                handlers[handled_type] = attr
        return handlers

    def start(self, host: ty.Any='localhost', port: int=8240) -> None:
        """
        Begins server loop.
        Will not exit without exception being thrown.
        :param host: address to serve
        :param port: port
        :return: int exit code
        """
        self.event_loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self.event_loop)
        asyncio.get_event_loop().run_until_complete(
            websockets.serve(self.receive, host, port))
        self._live = True
        asyncio.get_event_loop().run_forever()

    def end(self):
        if not self._live:
            raise RuntimeError(f'{self} is not currently live')
        if not isinstance(self.event_loop, asyncio.AbstractEventLoop):
            raise RuntimeError(f'{self} does not have an event loop to end')
        self.event_loop.stop()
        self._live = False

    async def receive(self, web_socket, path) -> None:
        logger = logging.getLogger(__name__)
        async for msg_str in web_socket:
            try:
                msg = e.decode_msg(msg_str)
            except e.MsgDecodeException as ex:
                logger.info('Could not decode recieved str', ex)
            else:
                await self.handle_msg(web_socket, msg)

    async def handle_msg(self, web_socket, msg: e.GameMsg) -> None:
        """
        Handles received GameMsg.

        Passed GameMsg is passed to the mapped sub-handler, and
        the result (if any) is then sent as a response to the sender.

        Exceptions thrown while handling messages will be logged
        instead of being allowed to end the server process.

        :param msg: GameMsg
        :return: Any
        """
        logger = logging.getLogger(__name__)
        logger.debug(f'Handling msg: {e}')
        # get handler
        try:
            msg_type: ty.Type = type(msg)
            sub_handler = self.handlers[msg_type]
        except KeyError as ex:
            logger.warning(
                f'No handler for event type: {type(msg)}', exc_info=ex)
            return

        # give msg to sub-handler
        try:
            await sub_handler(web_socket, msg)
        except Exception as ex:
            logger.warning(
                f'Exception thrown while handling {msg}', exc_info=ex)

    @handler(e.TestMsg)
    async def handle_test_msg(self, web_socket, msg: e.TestMsg) -> ty.Any:
        send_msg(web_socket, e.TestMsg(test_str=msg.test_str))

    @handler(e.ConnectRequestMsg)
    async def handle_connect_req(self, web_socket, msg: e.ConnectRequestMsg):
        logger = logging.getLogger(__name__)
        logger.info(f'New connection requested: {msg}')
        connection = Connection(web_socket, msg.requested_name)
        logger.info(f'connection accepted')
        self.connections[connection.id] = connection


class Connection:
    def __init__(self, web_socket, name: str, uuid: str=''):
        self.web_socket = web_socket
        self.name: str = name
        self.id: str = uuid or str(uuid4())

    async def send_msg(self, msg: e.GameMsg) -> None:
        msg_s: str = e.encode_msg(msg)
        await self.web_socket.send(msg_s)

    def __repr__(self) -> str:
        return f'Connection[{self.id[-8:]}]'
