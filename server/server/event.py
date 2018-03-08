import json
import uuid
import typing as ty


MSG_UUID_FIELD = 'uuid'
MSG_TYPE_FIELD = 'msg_type'  # must be identical to GameMsg class name


class GameMsg:
    def __init__(self, **kwargs):
        self.uuid: str = kwargs.get('uuid', str(uuid.uuid4()))

        for kw in filter(lambda kw_: kw_ not in self.__dict__, kwargs):
            raise ValueError(f'Unexpected, unused kwarg(s) passed: {kw}')

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
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.test_str = kwargs['test_str']

    @property
    def json_dict(self):
        d = super().json_dict
        d['test_str'] = self.test_str
        return d


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
        del d[MSG_TYPE_FIELD]
        return msg_type(**d)
