#! /bin/bash

cppcheck $1 --cppcheck-build-dir=$(dirname "$0")/build-cppcheck --check-level=exhaustive --force --xml 2>&1 | tee $(dirname "$0")/cppcheck-result.log