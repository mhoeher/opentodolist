import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Item {
    id: editor
    
    property TopLevelItem item: null
    property var library: null
    
    function __addTag() {
        if (newTagInput.displayText !== "")
        {
            editor.item.addTag(newTagInput.displayText);
            newTagInput.text = "";
            newTagInput.forceActiveFocus();
        }
    }
    
    height: childrenRect.height
    
    Column {
        width: parent.width
        spacing: Globals.defaultMargin
        
        Flow {
            id: tags
            
            property int __maxWidth: tags.width - Globals.minButtonHeight * 2 - (Globals.defaultMargin * 1.5)
            
            width: parent.width
            height: childrenRect.height
            flow: Flow.LeftToRight
            spacing: Globals.defaultMargin
            
            Repeater {
                model: editor.item.tags
                delegate: tagDelegate            
            }
            
            Row {
                spacing: Globals.defaultMargin / 2
                
                TextField {
                    id: newTagInput
                    
                    placeholderText: qsTr("Add tag...")
                    width: Math.min(tags.__maxWidth, Globals.fontPixelSize * 20)
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    Layout.fillWidth: true
                    Keys.onReturnPressed: editor.__addTag()
                    Keys.onEnterPressed: editor.__addTag()
                    onFocusChanged: {
                        existingTags.visible = focus;
                    }
                    anchors.verticalCenter: parent.verticalCenter
                    selectByMouse: true
                }
                Symbol {
                    symbol: Fonts.symbols.faPlus
                    onClicked: editor.__addTag()
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
        
        Label {
            id: existingTagsLabel
            
            font.bold: true
            text: qsTr("Or select an existing tag:")
            wrapMode: Text.WordWrap
            width: parent.width
            height: visible ? contentHeight : 0
            visible: existingTags.visible && (existingTagsRepeater.model.length > 0)
        }
        
        Flow {
            id: existingTags
            
            width: parent.width
            height: visible ? childrenRect.height : 0
            spacing: Globals.defaultMargin
            flow: Flow.LeftToRight
            visible: false
            
            Repeater {
                id: existingTagsRepeater
                
                model: {
                    var result = [];
                    if (library) {
                        for (var i = 0; i < library.tags.length; ++i) {
                            var attached = false;
                            for (var j = 0; j < item.tags.length; ++j) {
                                if (library.tags[i] === item.tags[j]) {
                                    attached = true;
                                    break;
                                }
                            }
                            if (!attached) {
                                result.push(library.tags[i]);
                            }
                        }
                    }
                    return result;
                }
                delegate: existingTagDelegate
            }
        }
    }
    
    Component {
        id: tagDelegate
        
        Rectangle {
            id: tagItem
            
            width: tagName.width + removeTag.width + Globals.defaultMargin * 1.5
            height: Math.max(tagName.height, removeTag.height, Globals.minButtonHeight)
            color: Colors.highlight
            border {
                width: 1
                color: Colors.mid
            }
            
            Label {
                id: tagName
                text: editor.item.tags[index]
                width: Math.min(contentWidth, tags.__maxWidth)
                color: Colors.highlightedText
                font.bold: true
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    margins: Globals.defaultMargin / 2
                }
            }
            
            Symbol {
                id: removeTag
                symbol: Fonts.symbols.faTimes
                color: Colors.highlightedText
                pixelSize: Globals.fontPixelSize
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: Globals.defaultMargin / 2
                }
                onClicked: editor.item.removeTagAt(index)
            }
        }
    }
    
    Component {
        id: existingTagDelegate
        
        MouseArea {
            width: childrenRect.width
            height: Math.max(childrenRect.height, Globals.minButtonHeight)
            
            onClicked: item.addTag(existingTagsRepeater.model[index])
            
            Label {
                text: existingTagsRepeater.model[index]
                font.bold: true
            }
        }
    }
}
