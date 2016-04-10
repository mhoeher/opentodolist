import QtQuick 2.5
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Panel {
    id: sidebar
    
    property var currentLibrary: App.libraries[0]
    
    signal libraryClicked(Library library)
    signal openLocalLibrary()
    
    contentItem: Item {
        
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
                    var library = App.libraries[index];
                    if (currentLibrary !== library) {
                        sidebar.currentLibrary = library;
                    }
                    libraryClicked(App.libraries[index]);
                    sidebar.close();
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
        
        ScrollView {
            anchors.fill: parent
            Column {
                width: sidebar.width
                
                ButtonContainer {
                    width: parent.width
                    opacity: 0.5
                    
                    Item {
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: Globals.defaultMargin
                            verticalCenter: parent.verticalCenter
                        }

                        height: Math.max(addLocalLibrarySymbol.height, addLocalLibraryLabel.height)
                        
                        Symbol {
                            id: addLocalLibrarySymbol
                            symbol: Fonts.symbols.faPlus
                            color: Colors.panelText
                            anchors {
                                left: parent.left
                                verticalCenter: parent.verticalCenter
                            }
                        }
                        Label {
                            id: addLocalLibraryLabel
                            text: qsTr("Add Local Library")
                            color: Colors.panelText
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            anchors {
                                left: addLocalLibrarySymbol.right
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                    onClicked: {
                        sidebar.close();
                        sidebar.openLocalLibrary();
                    }
                }
                
                Repeater {
                    id: librariesView
                    model: App.libraries
                    delegate: libraryItemDelegate
                    anchors.fill: parent
                }
            }
        }
    }
}
