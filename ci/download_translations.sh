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
        python3-pip \
        openssh-clients
fi

export QT_QPA_PLATFORM=minimal

pip install poeditor fire
./bin/poeditor-client.py upload $POEDITOR_TOKEN

./bin/po-to-ts.sh /usr/lib64/qt5/bin/lconvert
./bin/update-translations.sh /usr/lib64/qt5/bin/lupdate

if [ -n "$CI" ]; then
    eval $(ssh-agent -s)
    ssh-add "$OPENTODOLIST_DEPLOY_KEY"
    git config --global user.name "Martin Hoeher"
    git config --global user.email "martin@rpdev.net"
    git add app/translations
    git commit -m "Downloaded Translations from POEditor"
    git push "git@gitlab.com:rpdev/opentodolist.git"
fi
