import asyncio
import typing as ty
import websockets
import logging

from server import event as e
from server.model.model import GameModel


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
    Main server class that controls socket instance
    """
    def __init__(self):
        self.model = GameModel()
        self.handlers: ty.Dict[ty.Type, ty.Callable[[e.GameMsg], ty.Any]] = \
            self.find_handlers()

    def find_handlers(self) -> \
            ty.Dict[ty.Type, ty.Callable[[e.GameMsg], ty.Any]]:
        """
        Finds handlers among own attributes
        :return: dictionary of handlers, with key as handled type.
        """
        handlers: ty.Dict[ty.Type, ty.Callable[[e.GameMsg], ty.Any]] = {}
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
        asyncio.get_event_loop().run_until_complete(
            websockets.serve(self.handle_msg, host, port))
        asyncio.get_event_loop().run_forever()

    async def handle_msg(self, msg: e.GameMsg) -> None:
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
            logger.warning(f'No handler for event type: {msg}', ex)
            return

        # give msg to sub-handler
        try:
            result = sub_handler(msg)
        except Exception as ex:
            logger.warning(f'Exception thrown while handling {msg}', ex)

        # handle result returned by sub-handler

    @handler(e.TestMsg)
    def handle_test_msg(self, msg: e.TestMsg) -> ty.Any:
        pass
