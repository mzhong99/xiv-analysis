#!/usr/bin/python3

import contextlib
import argparse

import sys
import os

import multiprocessing

@contextlib.contextmanager
def push_directory(new_dir: str, create_if_absent=True):
    if create_if_absent:
        os.makedirs(new_dir, exist_ok=True)

    previous_dir = os.getcwd()
    os.chdir(new_dir)
    try:
        yield
    finally:
        os.chdir(previous_dir)

def main(args):
    default_build_directory = os.path.join(os.path.dirname(os.path.abspath(__file__)), "artifacts")

    parser = argparse.ArgumentParser(args)

    parser.add_argument("-c", "--clean", action="store_true",
                        help="Clean before building")
    parser.add_argument("-u", "--unittest", action="store_true",
                        help="Build and run unit tests")
    parser.add_argument("-d", "--directory", default=default_build_directory,
                        help="Directory to build project")
    parser.add_argument("-j", "--jobs", default=str(multiprocessing.cpu_count()),
                        help="Parallel jobs to use when building project")

    parsed_args = vars(parser.parse_args())

    with push_directory(parsed_args["directory"]):
        os.system("cmake ..")
        if parsed_args["clean"]:
            os.system("make clean")
        else:
            os.system("make -j{}".format(parsed_args["jobs"]))

if __name__ == "__main__":
    main(sys.argv)
