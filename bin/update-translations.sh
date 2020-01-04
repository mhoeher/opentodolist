#!/bin/bash

set -e

LUPDATE="$1"

if [ -z "$1" ]; then
    echo "No path to lupdate given!"
    exit 1
fi

cd "$(dirname "$(dirname "$(readlink -f "$0")")")"

"$LUPDATE" -no-obsolete opentodolist.pro
