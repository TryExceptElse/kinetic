from unittest import TestCase

import json

from server.event import GameMsg, MsgEncoder, msg_hook


class TestGameMsg(TestCase):

    def test_msg_can_be_dumped_to_json_and_then_loaded(self):
        msg_id: str = '123456789'
        msg: GameMsg = GameMsg(uuid=msg_id)
        msg_json: str = json.dumps(msg, cls=MsgEncoder)
        reconstructed_msg: GameMsg = json.loads(msg_json, object_hook=msg_hook)

        self.assertEqual(msg_id, reconstructed_msg.uuid)

    def test_msg_throws_value_exception_if_initialized_with_unused_kwarg(self):
        try:
            GameMsg(unused_kwarg='unused')
        except ValueError:
            pass
        else:
            self.fail('Exception should have been thrown when unused '
                      'kwarg was passed to GameMsg __init__.')
