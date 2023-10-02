#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

if [ -n "$CI" ]; then
    if [ ! -f /usr/bin/cppcheck ]; then
        dnf install -y --nogpgcheck cppcheck
    fi
fi

cppcheck  \
    --enable=warning,style,performance,portability \
    --disable=missingInclude \
    --error-exitcode=2 \
    --inline-suppr \
    --template='{file}:{line} {severity} "{id}": {message}' \
    --library=qt \
    --std=c++11 \
    --quiet \
    app lib
