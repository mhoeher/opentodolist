#!/bin/bash

set -e

SCRIPT_DIR="$(cd $(dirname "$0") && pwd)"
cd "$SCRIPT_DIR"
cd ..

if [ -n "$CI" ]; then
    dnf install -y \
        qt5-qtbase-devel \
        qt5-linguist \
        python3 \
        python3-pip
fi

export QT_QPA_PLATFORM=minimal

./bin/update-translations.sh /usr/lib64/qt5/bin/lupdate
./bin/ts-to-po.sh /usr/lib64/qt5/bin/lconvert

pip install poeditor fire
./bin/poeditor-client.py upload $POEDITOR_TOKEN
