#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

if [ -n "$CI" ]; then
    dnf install -y --nogpgcheck python python-pip git
    pip install git-archive-all
fi

OTL_VERSION=$(git describe --tags)
git-archive-all --force-submodules OpenTodoList-Source-$OTL_VERSION.zip 
git-archive-all --force-submodules OpenTodoList-Source-$OTL_VERSION.tar.gz
