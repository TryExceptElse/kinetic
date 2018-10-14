#!/usr/bin/env python3
import subprocess
import os


ACTOR_ROOT = os.path.dirname(os.path.dirname(__file__))
BUILD_PATH = os.path.join(ACTOR_ROOT, 'build_web')


def build_wasm() -> None:
    # Check if building is needed
    # If build is required, check that emscripten is installed.
    # If everything is ready for a build, run cmake
    if not os.path.exists(BUILD_PATH):
        os.mkdir(BUILD_PATH)
    os.chdir(BUILD_PATH)
    try:
        subprocess.run(('cmake', '..'))
    except FileNotFoundError as e:
        raise FileNotFoundError(
            'cmake does not appear to be installed. '
            'With ubuntu it may be installed with "sudo apt-get install cmake"'
        ) from e
    subprocess.run(('cmake', '--build', '.'))


if __name__ == '__main__':
    build_wasm()
