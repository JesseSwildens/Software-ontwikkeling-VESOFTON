#! /bin/bash

cppcheck $1 --cppcheck-build-dir=$(dirname "$0")/build-cppcheck --check-level=exhaustive --force --xml 2> $(dirname "$0")/cppcheck-result.xml