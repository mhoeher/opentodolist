import QtQuick 2.0
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList.UI 1.0

Item {
    id: wrapper
    
    property StackView stack: null
    property url fileUrl
    property url folder
    //property var nameFilters // list<string>
    property bool selectExisting
    property bool selectFolder
    property bool selectMultiple
    property string selectedNameFilter
    property var shortcuts
    property string sidebarVisible
    property string title
    //property bool visible
    property var fileNameExtensions
    
    signal accepted()
    
    function close() {
        if (stack) {
            stack.pop();
        }
    }
    
    function open() {
        stack.push(page)
    }
    
    Component {
        id: page
        
        FileSelectorPage {
            folder: wrapper.folder
            selectFolder: wrapper.selectFolder
            nameFilters: {
                var result = [];
                if (wrapper.fileNameExtensions) {
                    for (var i = 0; i < wrapper.fileNameExtensions.length; ++i) {
                        result.push("*." + wrapper.fileNameExtensions[i]);
                    }
                }
                return result;
            }
            
            onFileSelected: {
                if (!wrapper.selectFolder) {
                    wrapper.fileUrl = fileName;
                    wrapper.accepted();
                }
            }
            
            onFolderSelected: {
                if (wrapper.selectFolder) {
                    wrapper.folder = folderName;
                    wrapper.accepted();
                }
            }
        }
    }
}
