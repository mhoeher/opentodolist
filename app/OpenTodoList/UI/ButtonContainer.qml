import QtQuick 2.0

import OpenTodoList.UI 1.0

MouseArea {
    property Item mainItem: children[0]
    
    height: mainItem ? Math.max( mainItem.height, Globals.minButtonHeight) : Globals.minButtonHeight
    implicitWidth: Globals.minButtonHeight
}
