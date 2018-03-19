from uuid import uuid4


class GameObject:
    """
    Root game object class
    """

    def __init__(self, uuid: str='', name: str=''):

        self.id = uuid or str(uuid4())
        self.name = name
