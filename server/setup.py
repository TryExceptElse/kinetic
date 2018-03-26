from setuptools import setup, Extension
from Cython.Build import cythonize
from os import path

SERVER_PATH = path.abspath(path.dirname(__file__))
ROOT_PATH = path.abspath(path.join(SERVER_PATH, '..'))

setup(
    name='kinetic_server',
    version='0.0.0',
    packages=['server', 'server.model', 'test', 'test.model'],
    py_modules=['main', 'settings'],
    license='Apache License Version 2.0',
    author='TryExceptElse',
    description='The newtonian strategy game.',
    requires=['websockets', 'Cython'],

    ext_modules=cythonize([
        Extension(
            name='server.server',
            sources=['server/server.py']
        ),
        Extension(
            name='server.event',
            sources=['server/event.py']
        ),
        Extension(
            name='server.model.actor',
            sources=['server/model/actor.pyx'],
            include_dirs=[path.join(ROOT_PATH, 'actor', 'src')],
            library_dirs=[path.join(ROOT_PATH, 'actor', 'build', 'libs',
                                    'actor', 'static')],
            language="c++",
        ),
        Extension(
            name='server.model.vector',
            sources=['server/model/vector.pyx'],
            include_dirs=[path.join(ROOT_PATH, 'actor', 'src')],
            library_dirs=[path.join(ROOT_PATH, 'actor', 'build', 'libs',
                                    'actor', 'static')],
            language="c++",
        )
    ])
)
