#!/bin/bash

set -e

. secrets.env
podman build \
    --build-arg QT_CI_LOGIN="$QT_CI_LOGIN" \
    --build-arg QT_CI_PASSWORD="$QT_CI_PASSWORD" \
    --pull -t $1 .
