#!/bin/bash

set -e

cd "$(dirname "$(dirname "$(readlink -f "$0")")")"

docker run \
    -v "$PWD":/build \
    -w /build \
    snapcore/snapcraft:stable \
    snapcraft
