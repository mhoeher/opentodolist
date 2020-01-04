#!/bin/bash

set -e

cd "$(dirname "$(dirname "$(readlink -f "$0")")")"
cd app/translations

for file in *.po; do
    lconvert \
        -i $file \
        -o $(basename $file .po).ts
done
