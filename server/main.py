import sys

from server.server import Server
from settings import SERVER_ROOT


def main():
    """
    Main function that handles start of server function.
    :return: int
    """
    sys.path += [SERVER_ROOT]
    server = Server()
    server.start('127.0.0.1', 8240)


if __name__ == '__main__':
    main()
