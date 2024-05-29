#! /bin/bash

# Creating an empty file so nothing fails that expects a result file to be present
touch $(dirname "$0")/cppcheck-result.xml

# $n == argument n (0 == file, 1 == argument 1, etc.., $@ == all arguments)
# (>) redirects, 1 == stdout, 2 == stderr
cppcheck --cppcheck-build-dir=$(dirname "$0")/build-cppcheck --enable=all --check-level=exhaustive --error-exitcode=2 --xml $@ 2> $(dirname "$0")/cppcheck-result.xml