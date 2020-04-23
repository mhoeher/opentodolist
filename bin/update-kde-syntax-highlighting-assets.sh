#!/bin/bash

set -e

cd $(cd "$(dirname "$0")" && echo $PWD)
cd ..

PROJECT_HOME="$PWD"
ASSETS_DIR="$PROJECT_HOME/lib/qmake/kf5-syntax-highlighting"

function cleanup {
    rm -rf $PROJECT_HOME/tmp-syntax-highlighting-build
}

trap cleanup EXIT

# Create a temporary directory to build
# the syntax highlighting engine once:
rm -rf tmp-syntax-highlighting-build
mkdir -p tmp-syntax-highlighting-build
cd tmp-syntax-highlighting-build

# Configure and build:
cmake ../3rdparty/KDE/syntax-highlighting
make -j4

# Remove previous assets:
rm -rf $ASSETS_DIR/*

# Copy generated assets to the lib folder:
cp \
    src/lib/ksyntaxhighlighting_logging.cpp \
    src/lib/ksyntaxhighlighting_logging.h \
    src/lib/ksyntaxhighlighting_export.h \
    ksyntaxhighlighting_version.h \
    data/syntax-data.qrc \
    \
    $ASSETS_DIR

# The resource file which pulls in all the
# syntax definitions has absolute paths and also
# includes generated definitions in the build
# folder. Copy the generated definitions over as well
# and turn all file references into relative ones:
mkdir -p $ASSETS_DIR/data/generated/syntax
for file in $(\
    cat data/syntax-data.qrc | \
    grep $PWD | \
    tr '<>' ' ' | \
    awk '{ print $3; }'); do
    cp "$file" "$ASSETS_DIR${file#$PWD}"
done

sed \
    -e s#$PWD/data/#./data/#g \
    -e s#$PROJECT_HOME/#../../../#g \
    -i \
    $ASSETS_DIR/syntax-data.qrc
