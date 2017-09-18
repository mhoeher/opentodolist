import QtQuick 2.5
import QtQuick.Controls 2.2
import Qt.labs.folderlistmodel 2.1

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

ScrollView {
    id: root
    
    property string folder
    property bool selectFolder: false
    property alias nameFilters: folderListModel.nameFilters
    property var save: selectFolder ? __selectFolder : undefined
    property var goUp: App.folderExists(folderListModel.parentFolder) ? __goUp : undefined
    
    function goHome() {
        folderListModel.folder = App.homeLocation()
    }
    
    function cancel() {
    }
    
    function __selectFolder() {
        fileUrl = folderListModel.folder;
        folderSelected(folderListModel.folder);
        __cleanStack();
    }
    
    function __goUp() {
        folderListModel.folder = "file://" + folderListModel.parentFolder
    }
    
    function __cleanStack() {
        if (stack.currentItem === root) {
            stack.pop();
        }
    }
        

    signal fileSelected(url fileName)
    signal folderSelected(url folderName)
    
    Component {
        id: itemDelegate
        
        MouseArea {
            height: Math.max(symbol.height, text.height)
            width: parent.width
            onClicked: {
                if (fileIsDir) {
                    folderListModel.folder = "file://" + filePath;
                    console.warn("file://" + filePath);
                } else {
                    root.fileSelected("file://" + filePath)
                    __cleanStack();
                }
            }
            
            Symbol {
                id: symbol
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    margins: Globals.defaultMargin
                }
                symbol: fileIsDir ? Fonts.symbols.faFolderOpenO : Fonts.symbols.faFileO
            }
            Text {
                id: text
                anchors {
                    left: symbol.right
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: Globals.defaultMargin
                }
                text: fileName
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
        }
    }
    
    ListView {
        id: view
        anchors.fill: parent
        
        model: FolderListModel {
            id: folderListModel
            
            sortField: FolderListModel.Name
            showDirs: true
            showFiles: !selectFolder
            showDotAndDotDot: false
            showDirsFirst: true
            showOnlyReadable: false
            showHidden: true
            folder: App.homeLocation()
        }
        delegate: itemDelegate
    }
}
