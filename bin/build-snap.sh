#!/bin/bash

set -e

cd "$(dirname "$(dirname "$(readlink -f "$0")")")"

podman run \
    -v "$PWD":/build \
    -w /build \
    snapcore/snapcraft:stable \
    snapcraft
