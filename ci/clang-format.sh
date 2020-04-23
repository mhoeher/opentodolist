#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

if [ -n "$CI" ]; then
    if [ ! -f /usr/bin/clang-format ]; then
        dnf install -y --nogpgcheck clang
    fi
    if [ ! -f /usr/bin/python ]; then
        dnf install -y --nogpgcheck python
    fi
fi

./3rdparty/run-clang-format/run-clang-format.py \
    -r app lib test \
    --exclude lib/qmake/kf5-syntax-highlighting
