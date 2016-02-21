import QtQuick 2.5

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Panel {
    id: sidebar
    
    property var currentLibrary: null
    
    signal libraryClicked(Library library)
    
    Connections {
        target: currentLibrary
        onLibraryDeleted: currentLibrary = null
    }

    Component {
        id: libraryItemDelegate
        
        ButtonContainer {
            mainItem: itemText
            width: parent.width
            cursorShape: Qt.PointingHandCursor
            
            onClicked: {
                sidebar.currentLibrary = App.libraries[index]
                libraryClicked(App.libraries[index])
            }
            
            Rectangle {
                anchors.fill: parent
                color: Colors.secondary3
                visible: opacity > 0.0
                opacity: currentLibrary === App.libraries[index]
                
                Behavior on opacity { NumberAnimation { duration: 500 } }
            }
            
            Text {
                id: itemText
                text: name
                color: Colors.panelText
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: Globals.defaultMargin
                }
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
        }
    }
    
    ListView {
        id: librariesView
        model: App.libraries
        delegate: libraryItemDelegate
        anchors.fill: parent
    }
}
