import sys
from setuptools import setup, Extension
from Cython.Build import cythonize
from os import path

SERVER_PATH = path.abspath(path.dirname(__file__))
ROOT_PATH = path.abspath(path.join(SERVER_PATH, '..'))

DEFAULT_INCLUDES = [
    path.join(ROOT_PATH, 'actor', 'src'),
    path.join(ROOT_PATH, 'actor', 'third_party', 'src')
]


def static_lib_name(base_name: str) -> str:
    if sys.platform == 'win32':
        raise OSError('win32 not supported currently.')
    else:
        return f'lib{base_name}.a'


DEFAULT_OBJECTS = [path.join(ROOT_PATH, 'actor', 'build', 'libs',
                             'actor', 'static', static_lib_name('actor'))]

DEFAULT_COMPILER_ARGS = ['-O0', '-g']

SCRIPT_MODULES = [
    'scripts.orbit_vis',
]


setup(
    name='kinetic_server',
    version='0.0.0',
    packages=['server', 'server.model', 'test', 'test.model'],
    py_modules=['main', 'settings'] + SCRIPT_MODULES,
    license='Apache License Version 2.0',
    author='TryExceptElse',
    description='The newtonian strategy game.',
    requires=['websockets', 'Cython'],

    ext_modules=cythonize([
        # Extension(
        #     name='server.server',
        #     sources=['server/server.py']
        # ),
        Extension(
            name='server.event',
            sources=['server/event.py']
        ),
        Extension(
            name='server.model.actor',
            sources=['server/model/actor.pyx'],
            include_dirs=DEFAULT_INCLUDES,
            extra_objects=DEFAULT_OBJECTS,
            extra_compile_args=DEFAULT_COMPILER_ARGS,
            language="c++",
        ),
        Extension(
            name='server.model.vector',
            sources=['server/model/vector.pyx'],
            include_dirs=DEFAULT_INCLUDES,
            extra_compile_args=DEFAULT_COMPILER_ARGS,
            language="c++",
        ),
        Extension(
            name='server.model.path',
            sources=['server/model/path.pyx'],
            include_dirs=DEFAULT_INCLUDES,
            extra_objects=DEFAULT_OBJECTS,
            extra_compile_args=DEFAULT_COMPILER_ARGS,
            language='c++'
        ),
        Extension(
            name='server.model.body',
            sources=['server/model/body.pyx'],
            include_dirs=DEFAULT_INCLUDES,
            extra_objects=DEFAULT_OBJECTS,
            extra_compile_args=DEFAULT_COMPILER_ARGS,
            language='c++'
        ),
        Extension(
            name='server.model.orbit',
            sources=['server/model/orbit.pyx'],
            include_dirs=DEFAULT_INCLUDES,
            extra_objects=DEFAULT_OBJECTS,
            extra_compile_args=DEFAULT_COMPILER_ARGS,
            language='c++'
        ),
        Extension(
            name='server.model.system',
            sources=['server/model/system.pyx'],
            include_dirs=DEFAULT_INCLUDES,
            extra_objects=DEFAULT_OBJECTS,
            extra_compile_args=DEFAULT_COMPILER_ARGS,
            language='c++'
        ),
        Extension(
            name='server.model.universe',
            sources=['server/model/universe.pyx'],
            include_dirs=DEFAULT_INCLUDES,
            extra_objects=DEFAULT_OBJECTS,
            extra_compile_args=DEFAULT_COMPILER_ARGS,
            language='c++'
        )
    ], gdb_debug=True)
)
