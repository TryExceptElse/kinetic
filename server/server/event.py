import json
import typing as ty

from uuid import uuid4


MSG_UUID_FIELD = 'uuid'
MSG_TYPE_FIELD = 'msg_type'  # must be identical to GameMsg class name


class GameMsg:
    """
    Parent class for all messages exchanged between client and server.
    Intended to be converted to json and passed over socket.
    """
    def __init__(self, uuid: str=''):
        self.uuid: str = uuid or uuid4()

    @property
    def json_dict(self):
        """
        Gets dictionary of attributes to be represented in json.
        :return: dict
        """
        return {
            MSG_UUID_FIELD: self.uuid,
            MSG_TYPE_FIELD: self.__class__.__name__
        }


class TestMsg(GameMsg):
    """
    Simple Msg class extending GameMsg for misc. testing purposes, both
    for testing functionality during development, and connection during
    deployment.
    """
    def __init__(self, msg_id: str='', test_str: str=''):
        super().__init__(msg_id)
        self.test_str = test_str

    @property
    def json_dict(self):
        d = super().json_dict
        d['test_str'] = self.test_str
        return d


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
