#!/usr/bin/env python3
"""
Script for getting needed dependencies and any other pre-build
tasks done.
"""
import os
import shutil
import requests
import io
import zipfile

ACTOR_ROOT = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
THIRD_PARTY_DIR = os.path.join(ACTOR_ROOT, 'third_party', 'src')
EIGEN_INCLUDE_DIR_NAME = 'Eigen'
EIGEN_DIR = os.path.join(THIRD_PARTY_DIR, EIGEN_INCLUDE_DIR_NAME)

EIGEN_URL = 'https://github.com/eigenteam/eigen-git-mirror/archive/3.3.4.zip'


def git_zip_base_name(url: str) -> str:
    path_elements = os.path.splitext(EIGEN_URL)[0].split('/')
    return path_elements[-3] + '-' + path_elements[-1]


def get_eigen():
    print('Getting Eigen...')
    response = requests.get(EIGEN_URL)
    root_dir_name = git_zip_base_name(EIGEN_URL)
    with zipfile.ZipFile(io.BytesIO(response.content)) as eigen_zip:
        print('Extracting...')
        for file_name in eigen_zip.namelist():
            if file_name.startswith(os.path.join(
                    root_dir_name, EIGEN_INCLUDE_DIR_NAME)):
                eigen_zip.extract(file_name, THIRD_PARTY_DIR)

    # copy
    print(f'Moving Include dir to {EIGEN_DIR}')
    shutil.move(
        os.path.join(THIRD_PARTY_DIR, root_dir_name, EIGEN_INCLUDE_DIR_NAME),
        os.path.join(THIRD_PARTY_DIR, EIGEN_INCLUDE_DIR_NAME)
    )
    os.rmdir(os.path.join(THIRD_PARTY_DIR, root_dir_name))
    # Remove CMakeList because it shouldn't be there.
    os.remove(os.path.join(EIGEN_DIR, 'CMakeLists.txt'))
    print('Done.')


def main():
    if not os.path.exists(EIGEN_DIR):
        print('Getting Eigen include dir because it was not found.')
        get_eigen()


if __name__ == '__main__':
    main()
