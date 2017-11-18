#!/bin/bash
mkdir build-macos
cd build-macos
/Users/mhoeher/Qt/5.9.2/clang_64/bin/qmake CONFIG+=release CONFIG+=with_update_service ..
make -j2
/Users/mhoeher/Qt/5.9.2/clang_64/bin/macdeployqt app/OpenTodoList.app/ -dmg -qmldir=../app
