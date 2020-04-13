#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

if [ -n "$CI" ]; then
    if [ ! -f /usr/bin/python ]; then
        dnf install -y --nogpgcheck python
    fi
    if [ ! -f /usr/bin/pip ]; then
        dnf install -y --nogpgcheck pip
    fi
    if ! command -v cpplint ; then
        pip install cpplint
    fi
fi

# cpplint suggests include guards including the file hierarchy.
# Forwever, for this to work "as expected", we need to run it inside the
# "local" root directory, i.e. otherwise it will include e.g. the `LIB_` part
# for everything in the library:

success=0

for dir in app lib test; do
    pushd $dir
    if ! cpplint \
        --recursive \
        --filter=-whitespace,-build/include_subdir,-readability/todo,-runtime/references \
        --linelength=100 \
        --quiet \
        --root=. \
        .; then
        success=1
    fi
    popd
done

exit $success
