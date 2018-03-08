import json
import uuid
import typing as ty


MSG_UUID_FIELD = 'uuid'
MSG_TYPE_FIELD = 'msg_type'  # must be identical to GameMsg class name


class GameMsg:
    def __init__(self, msg_id: str=''):
        self.uuid: str = msg_id or str(uuid.uuid4())

    @classmethod
    def from_json_dict(cls, d: dict) -> 'GameMsg':
        """
        Re-creates GameMsg from dictionary produced from json.
        :param d: dict
        :return: GameMsg
        """
        msg_id: str = d[MSG_UUID_FIELD]
        return GameMsg(msg_id)

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


class MsgEncoder(json.JSONEncoder):
    def default(self, o: ty.Any) -> ty.Any:
        """
        Converts passed object to be encoded into an object that can
        be json-encoded.
        :param o: dict
        :return:
        """
        if isinstance(o, GameMsg):
            return o.json_dict
        return super().default(o)


def msg_hook(d: dict) -> 'GameMsg':
    try:
        msg_type = globals()[d[MSG_TYPE_FIELD]]
    except KeyError as e:
        raise ValueError(f'Could not find GameMsg type {d[MSG_TYPE_FIELD]}', e)
    else:
        return msg_type.from_json_dict(d)
