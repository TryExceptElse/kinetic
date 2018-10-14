# cython: infer_types=True, language_level=3

import json
import typing as ty

from uuid import uuid4


MSG_UUID_FIELD = 'uuid'
MSG_TYPE_FIELD = 'msg_type'  # must be identical to GameMsg class name


class MsgDecodeException(Exception):
    def __init__(self, err_msg: str, s: str, *args, **kwargs):
        err_msg += 'encoded str: ' + s
        super(err_msg, *args, **kwargs)
        self.encoded_string = s


class GameMsg:
    """
    Parent class for all messages exchanged between client and server.
    Intended to be converted to json and passed over socket.
    """
    def __init__(self, uuid: str=''):
        self.uuid: str = uuid or str(uuid4())

    @property
    def json_dict(self) -> ty.Dict[str, ty.Any]:
        """
        Gets dictionary of attributes to be represented in json.
        The implementation in GameMsg does nothing special, but
        may be extended by subclasses.

        values that contain None are not stored in json.
        :return: dict
        """
        d: ty.Dict[str, ty.Any] = self.__dict__
        d[MSG_TYPE_FIELD] = self.__class__.__name__
        return d

    def __repr__(self) -> str:
        return f'{self.__class__.__name__}[\'{self.uuid[-8:]}]'


class TestMsg(GameMsg):
    """
    Simple Msg class extending GameMsg for misc. testing purposes, both
    for testing functionality during development, and connection during
    deployment.
    """
    def __init__(self, msg_id: str='', test_str: str=''):
        super().__init__(msg_id)
        self.test_str = test_str


class ConnectRequestMsg(GameMsg):
    """
    Message sent from client to server requesting registration
    with the server.
    """
    def __init__(self, requested_name: str, uuid: str=''):
        super().__init__(uuid=uuid)
        self.requested_name: str = requested_name


class ConnectConfirmMsg(GameMsg):
    """
    Message sent from server to client confirming registration,
    and giving the client a uuid to use to identify itself.
    """
    def __init__(self, registered_name: str, uuid: str):
        super().__init__()
        self.registered_name = registered_name
        self.uuid = uuid


class MsgEncoder(json.JSONEncoder):
    """
    Handles conversion of GameMsg to json
    """
    def default(self, o: ty.Any) -> ty.Any:
        """
        Converts passed object to be encoded into an object that can
        be json-encoded.
        If passed object is not a recognized type,
        JSONEncoder default is used.
        :param o: Any
        :return: Any
        """
        if isinstance(o, GameMsg):
            return o.json_dict
        return super().default(o)


def msg_hook(d: ty.Dict[str, ty.Any]) -> 'GameMsg':
    """
    Handles conversion from json dict into GameMsg

    This function is intended to be used as the object_hook
    in json.loads().
    :param d: dict created from json
    :return: GameMsg
    :raises ValueError if msg type specified by json could not be found.
    """
    try:
        msg_type = globals()[d[MSG_TYPE_FIELD]]
    except KeyError as e:
        raise ValueError(f'Could not find GameMsg type {d[MSG_TYPE_FIELD]}', e)
    else:
        del d[MSG_TYPE_FIELD]  # remove now-unused field from passed kwargs
        return msg_type(**d)


def encode_msg(msg: GameMsg) -> str:
    if not isinstance(msg, GameMsg):
        raise TypeError(f'Expected GameMsg, got {msg}')
    return json.dumps(msg, cls=MsgEncoder)


def decode_msg(msg_str: str) -> GameMsg:
    o: ty.Any = json.loads(msg_str, object_hook=msg_hook)
    if not isinstance(o, GameMsg):
        raise MsgDecodeException(
            'Passed json was not a GameMsg object', msg_str)
    return o
