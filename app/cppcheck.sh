#! /bin/bash

# Creating an empty file so nothing fails that expects a result file to be present
touch $(dirname "$0")/cppcheck-result.xml

cppcheck --version | echo
# $n == argument n (0 == file, 1 == argument 1, etc.., $@ == all arguments)
# (>) redirects, 1 == stdout, 2 == stderr
cppcheck --cppcheck-build-dir=$(dirname "$0")/build-cppcheck --check-level=exhaustive --xml $@ 2> $(dirname "$0")/cppcheck-result.xml

result=$(<$(dirname "$0")/cppcheck-result.xml)

if [[ $result == *"<error "* ]] || [[ $result == *"<error>"* ]]; then
    # Error is detected in xml file
    echo $result
    exit 1 # Try again ;)
else
    exit 0
fi