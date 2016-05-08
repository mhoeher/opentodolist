import QtQuick 2.5
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0

import "MainWindowLogic.js" as Logic

Menu {
    id: formatMenu
    title: qsTr("Format")
    
    property MenuItem defaultStyle: defaultStyle
    property MenuItem heading1: heading1
    property MenuItem heading2: heading2
    property MenuItem heading3: heading3
    property MenuItem heading4: heading4
    property MenuItem heading5: heading5
    property MenuItem heading6: heading6
    property MenuItem code: code
    property MenuItem boldItem: bold
    property MenuItem italicItem: italic
    property MenuItem underlineItem: underline
    property MenuItem strikethroughItem: strikethrough
    property MenuItem bulletListItem: bulletList
    property MenuItem orderedListItem: orderedList
    property MenuItem indentItem: indent
    property MenuItem outdentItem: outdent
    property MenuItem alignLeftItem: alignLeft
    property MenuItem alignCenterItem: alignCenter
    property MenuItem alignRightItem: alignRight
    property MenuItem alignJustifyItem: alignJustify
    
    property var __item: activeFocusItem
    property var __itemFormatter: Logic.richTextEditorForItem(__item)
    
    property var __defaultFormatter: QtObject {}
    property var __formatter: __itemFormatter || __defaultFormatter
    
    property var __textSelected: __formatter && 
                                 __formatter.selectionStart != __formatter.selectionEnd
    
    visible: __formatter !== __defaultFormatter
    
    Menu {
        title: qsTr("Style")
        
        MenuItem {
            id: defaultStyle
            text: qsTr("Default")
            shortcut: qsTr("Ctrl+Shift+D")
            onTriggered: __formatter.defaultStyle()
        }
        
        MenuItem {
            id: heading1
            text: qsTr("Title 1")
            shortcut: qsTr("Ctrl+Shift+1")
            onTriggered: __formatter.heading1()
        }
        
        MenuItem {
            id: heading2
            text: qsTr("Title 2")
            shortcut: qsTr("Ctrl+Shift+2")
            onTriggered: __formatter.heading2()
        }
        
        MenuItem {
            id: heading3
            text: qsTr("Title 3")
            shortcut: qsTr("Ctrl+Shift+3")
            onTriggered: __formatter.heading3()
        }
        
        MenuItem {
            id: heading4
            text: qsTr("Title 4")
            shortcut: qsTr("Ctrl+Shift+4")
            onTriggered: __formatter.heading4()
        }
        
        MenuItem {
            id: heading5
            text: qsTr("Title 5")
            shortcut: qsTr("Ctrl+Shift+5")
            onTriggered: __formatter.heading5()
        }
        
        MenuItem {
            id: heading6
            text: qsTr("Title 6")
            shortcut: qsTr("Ctrl+Shift+6")
            onTriggered: __formatter.heading6()
        }
        
        MenuItem {
            id: code
            text: qsTr("Code")
            shortcut: qsTr("Ctrl+Shift+C")
            onTriggered: __formatter.code()
        }
    }
    
    MenuSeparator {}
    
    MenuItem {
        id: bold
        text: qsTr("Bold")
        shortcut: StandardKey.Bold
        onTriggered: __formatter.toggleBold()
    }
    
    MenuItem {
        id: italic
        text: qsTr("Italic")
        shortcut: StandardKey.Italic
        onTriggered: __formatter.toggleItalic()
    }
    
    MenuItem {
        id: underline
        text: qsTr("Underline")
        shortcut: StandardKey.Underline
        onTriggered: __formatter.toggleUnderline()
    }
    
    MenuItem {
        id: strikethrough
        text: qsTr("Strikethrough")
        onTriggered: __formatter.toggleStrikeThrough()
    }
    
    MenuSeparator {}
    
    MenuItem {
        id: bulletList
        text: qsTr("Bullet List")
        onTriggered: __formatter.insertUnorderedList()
    }
    
    MenuItem {
        id: orderedList
        text: qsTr("Ordered List")
        onTriggered: __formatter.insertOrderedList()
    }
    
    MenuSeparator {}
    
    MenuItem {
        id: indent
        text: qsTr("Indent")
        onTriggered: __formatter.indent()
    }
    
    MenuItem {
        id: outdent
        text: qsTr("Outdent")
        onTriggered: __formatter.outdent()
    }
    
    MenuSeparator {}
    
    MenuItem {
        id: alignLeft
        text: qsTr("Align Left")
        onTriggered: __formatter.alignLeft()
    }
    
    MenuItem {
        id: alignCenter
        text: qsTr("Center")
        onTriggered: __formatter.alignCenter()
    }
    
    MenuItem {
        id: alignRight
        text: qsTr("Align Right")
        onTriggered: __formatter.alignRight()
    }
    
    MenuItem {
        id: alignJustify
        text: qsTr("Justify")
        onTriggered: __formatter.alignJustify()
    }
    
}
