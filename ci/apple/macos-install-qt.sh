#!/bin/bash

set -e

if [ -d "$HOME/Qt" ]; then
    echo "Qt is already installed... skipping installation!"
    exit 0
fi

if [ -n "$CI" ]; then
    QT_ARCHIVE_MACOS="/tmp/Qt-MacOS.zip"
    QT_ARCHIVE_IOS="/tmp/Qt-iOS.zip"
    QT_ARCHIVE_TOOLS="/tmp/Qt-Tools.zip"
    # 46171955 == Project ID of https://gitlab.com/rpdev/packages/qt6
    QT_URL_MACOS="https://gitlab.com/api/v4/projects/46171955/packages/generic/Qt6/$QT_VERSION/Qt-MacOS.zip"
    QT_URL_IOS="https://gitlab.com/api/v4/projects/46171955/packages/generic/Qt6/$QT_VERSION/Qt-iOS.zip"
    QT_URL_TOOLS="https://gitlab.com/api/v4/projects/46171955/packages/generic/Qt6/$QT_VERSION/Qt-Tools.zip"

    curl -L -o "$QT_ARCHIVE_MACOS" "$QT_URL_MACOS"
    curl -L -o "$QT_ARCHIVE_TOOLS" "$QT_URL_TOOLS"
    if [ -n "$QT_INSTALL_IOS" ]; then
        curl -L -o "$QT_ARCHIVE_IOS" "$QT_URL_IOS"
    fi
    
    cd $HOME
    mkdir -p Qt
    unzip -q $QT_ARCHIVE_MACOS
    unzip -q $QT_ARCHIVE_TOOLS
    if [ -n "$QT_INSTALL_IOS" ]; then
        unzip -q $QT_ARCHIVE_IOS
    fi
fi
