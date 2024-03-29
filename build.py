#!/usr/bin/python3

import contextlib
import argparse

import sys
import os
import shutil

import multiprocessing

def find_file(name, path):
    for root, dirs, files in os.walk(path):
        if name in files:
            result = os.path.join(root, name)
            print("Use oodle library {}".format(result))
            return result
    print("Could not find a valid {} in {} - exiting...".format(name, path))
    exit()

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

def run_and_check(command: str, expect=0):
    rc = os.system(command)
    if rc != expect:
        print("{} returned {} but expected {}".format(command, rc, expect))
        exit(rc)

def main(args):
    repository_root = os.path.dirname(os.path.abspath(__file__))
    default_build_directory = os.path.join(repository_root, "artifacts")

    parser = argparse.ArgumentParser(args)

    parser.add_argument("-c", "--clean", action="store_true", help="Clean before building")
    parser.add_argument("-u", "--unittest", action="store_true", help="Build and run unit tests")
    parser.add_argument("-m", "--memtest", action="store_true", help="Memory leak detection (unit testing)")
    parser.add_argument("-d", "--directory", default=default_build_directory, help="Directory to build project")
    parser.add_argument("-j", "--jobs", default=str(multiprocessing.cpu_count()), help="Parallel compilation job count")

    parsed_args = vars(parser.parse_args())
    run_and_check("ctags -R --c++-kinds=+p --fields=+iaS --extras=+q --language-force=C++ {}".format(repository_root))

    with push_directory(parsed_args["directory"]):
        run_and_check("cd .. && git submodule update --init --recursive")

        oo2core_dll = find_file("oo2core_9_win64.dll", os.path.expanduser("~"))
        oo2net_dll = find_file("oo2net_9_win64.dll", os.path.expanduser("~"))

        run_and_check("cp {} .".format(oo2core_dll))
        run_and_check("cp {} .".format(oo2net_dll))

        run_and_check("cmake ..")

        if parsed_args["clean"]:
            run_and_check("make clean")

        run_and_check("make -j{}".format(parsed_args["jobs"]))
        if parsed_args["unittest"]:
            memtest_util = ""
            if parsed_args["memtest"]:
                memtest_util = "valgrind"
            run_and_check("{} {} --duration".format(memtest_util, os.path.join(parsed_args["directory"], "unittests")))

if __name__ == "__main__":
    main(sys.argv)
