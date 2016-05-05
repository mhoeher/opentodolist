import QtQuick 2.5
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Panel {
    id: sidebar
    
    property var currentLibrary: App.libraries[0]
    property string currentTag: ""
    property bool helpVisible: false
    
    signal libraryClicked(Library library)
    signal openLocalLibrary()
    signal aboutPageRequested()
    
    contentItem: Item {
        
        Connections {
            target: currentLibrary
            onLibraryDeleted: currentLibrary = null
        }
        
        Component {
            id: libraryItemDelegate
            
            Column {
                width: parent.width
                
                ButtonContainer {
                    mainItem: itemText
                    width: parent.width
                    cursorShape: Qt.PointingHandCursor
                    
                    onClicked: {
                        var library = App.libraries[index];
                        if (currentLibrary !== library) {
                            sidebar.currentLibrary = library;
                        }
                        sidebar.currentTag = "";
                        libraryClicked(App.libraries[index]);
                        sidebar.close();
                    }
                    
                    Rectangle {
                        anchors.fill: parent
                        color: Colors.secondary3
                        visible: opacity > 0.0
                        opacity: (currentLibrary === App.libraries[index]) &&
                                 (currentTag === "")
                        
                        Behavior on opacity { NumberAnimation { duration: 500 } }
                    }
                    
                    Text {
                        id: itemText
                        text: name
                        color: Colors.panelText
                        font.bold: true
                        anchors {
                            left: parent.left
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            margins: Globals.defaultMargin
                        }
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    }
                }
                
                Repeater {
                    id: tags
                    
                    property var library: App.libraries[index]
                    
                    model: library.tags
                    delegate: tag
                    
                    Component {
                        id: tag
                        
                        ButtonContainer {
                            mainItem: tagText
                            width: parent.width
                            cursorShape: Qt.PointingHandCursor
                            
                            onClicked: {
                                if (currentLibrary !== tags.library) {
                                    sidebar.currentLibrary = tags.library;
                                }
                                if (sidebar.currentTag !== tags.library.tags[index]) {
                                    sidebar.currentTag = tags.library.tags[index];
                                }
                                libraryClicked(tags.library);
                                sidebar.close();
                            }
                            
                            Rectangle {
                                anchors.fill: parent
                                color: Colors.secondary3
                                visible: opacity > 0.0
                                opacity: (currentLibrary === tags.library) && 
                                         (currentTag === tags.library.tags[index])
                                
                                Behavior on opacity { NumberAnimation { duration: 500 } }
                            }
                            
                            Text {
                                id: tagText
                                text: tags.library.tags[index]
                                color: Colors.panelText
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    verticalCenter: parent.verticalCenter
                                    margins: Globals.defaultMargin
                                    leftMargin: Globals.defaultMargin * 2
                                }
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }
                        }
                    }
                }
            }
        }
        
        ScrollView {
            id: scrollView
            anchors.fill: parent
            horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
            
            Column {
                width: scrollView.width
                
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
                
                ButtonContainer {
                    width: parent.width
                    mainItem: showAboutItem
                    
                    Rectangle {
                        anchors.fill: parent
                        color: Colors.secondary3
                        visible: opacity > 0.0
                        opacity: sidebar.helpVisible ? 1.0 : 0.0
                        
                        Behavior on opacity { NumberAnimation { duration: 500 } }
                    }
                    
                    Item {
                        id: showAboutItem
                        
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: Globals.defaultMargin
                            verticalCenter: parent.verticalCenter
                        }
                        
                        height: Math.max(helpSymbol.height, helpText.height) + Globals.defaultMargin
                        
                        Symbol {
                            id: helpSymbol
                            symbol: Fonts.symbols.faInfo
                            color: Colors.panelText
                            anchors {
                                left: parent.left
                                verticalCenter: parent.verticalCenter
                            }
                        }
                        Label {
                            id: helpText
                            text: qsTr("About...")
                            color: Colors.panelText
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            anchors {
                                left: helpSymbol.right
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                    onClicked: {
                        sidebar.aboutPageRequested();
                        sidebar.close();
                    }
                }
            }
        }
    }
    
    
}
