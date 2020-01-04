#!/bin/bash

set -e

LCONVERT="$1"

if [ -z "$1" ]; then
    LCONVERT=lconvert
fi

cd "$(dirname "$(dirname "$(readlink -f "$0")")")"
cd app/translations

for file in *.po; do
    "$LCONVERT" \
        -i $file \
        -o $(basename $file .po).ts
done
