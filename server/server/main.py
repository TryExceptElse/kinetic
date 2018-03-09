from .server import Server


def main():
    """
    Main function that handles start of server function.
    :return: int
    """
    server = Server()
    server.start('127.0.0.1', 8240)


if __name__ == '__main__':
    main()
