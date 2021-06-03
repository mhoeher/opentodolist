#!/bin/bash

set -e

LCONVERT="$1"

if [ -z "$1" ]; then
    LCONVERT=lconvert
fi

cd "$(dirname "$(dirname "$(readlink -f "$0")")")"
cd app/translations

for file in *.po; do
    target_lang="$(echo "$file" | tr '.-' ' ' | cut -d' ' -f2)"
    "$LCONVERT" \
        -i $file \
        -o $(basename $file .po).ts \
        --target-language "$target_lang"
done
