#! /bin/bash

# $n == argument n (0 == file, 1 == argument 1, etc..)
# (>) redirects, 1 == stdout, 2 == stderr
cppcheck $1 --cppcheck-build-dir=$(dirname "$0")/build-cppcheck --force --xml 2> $(dirname "$0")/cppcheck-result.xml

if [ -s $(dirname "$0")/cppcheck-result.log ]; then
    # Something is inside the error result file
    echo $(<$(dirname "$0")/cppcheck-result.log)
    exit 1 # Try again ;)
else
    exit 0
fi