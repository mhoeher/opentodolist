import QtQuick 2.0

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

StickyNote {
    property ImageTopLevelItem libraryItem: ImageTopLevelItem {}
    
    title: libraryItem.title
    text: libraryItem.notes
    clip: true
    backgroundColor: Colors.itemColor(libraryItem.color)
    hoverEnabled: true
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    image: libraryItem.validImage ? libraryItem.directory + "/" + libraryItem.image : ""
}

