from distutils.core import setup

setup(
    name='kinetic',
    version='0.0.0',
    packages=['test', 'server'],
    package_dir={'': 'server'},
    license='Apache License Version 2.0',
    author='TryExceptElse',
    description='The newtonian strategy game.', requires=['websockets']
)
