import QtQuick 2.5
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0

import "MainWindowLogic.js" as Logic

Menu {
    id: formatMenu
    title: qsTr("Format")
    
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
    
    property int paragraphStyle: __formatter.paragraphStyle
    
    onParagraphStyleChanged: console.debug(paragraphStyle)
    
    function applyParagraphStyle(style) {
        __formatter.paragraphStyle = style;
    }
    
    property var __item: activeFocusItem
    property var __itemFormatter: Logic.documentFormatterForItem(__item)
    
    property DocumentFormatter __defaultFormatter: DocumentFormatter {}
    property DocumentFormatter __formatter: __itemFormatter || __defaultFormatter
    
    visible: __formatter !== __defaultFormatter
    
    Menu {
        title: qsTr("Style")
        
        MenuItem {
            text: qsTr("Default")
            checkable: true
            checked: __formatter.paragraphStyle === DocumentFormatter.Default
            shortcut: qsTr("Ctrl+Shift+D")
            onTriggered: __formatter.paragraphStyle = DocumentFormatter.Default
        }
        
        MenuItem {
            text: qsTr("Title 1")
            checkable: true
            checked: __formatter.paragraphStyle === DocumentFormatter.H1
            shortcut: qsTr("Ctrl+Shift+1")
            onTriggered: __formatter.paragraphStyle = DocumentFormatter.H1
        }
        
        MenuItem {
            text: qsTr("Title 2")
            checkable: true
            checked: __formatter.paragraphStyle === DocumentFormatter.H2
            shortcut: qsTr("Ctrl+Shift+2")
            onTriggered: __formatter.paragraphStyle = DocumentFormatter.H2
        }
        
        MenuItem {
            text: qsTr("Title 3")
            checkable: true
            checked: __formatter.paragraphStyle === DocumentFormatter.H3
            shortcut: qsTr("Ctrl+Shift+3")
            onTriggered: __formatter.paragraphStyle = DocumentFormatter.H3
        }
        
        MenuItem {
            text: qsTr("Title 4")
            checkable: true
            checked: __formatter.paragraphStyle === DocumentFormatter.H4
            shortcut: qsTr("Ctrl+Shift+4")
            onTriggered: __formatter.paragraphStyle = DocumentFormatter.H4
        }
        
        MenuItem {
            text: qsTr("Title 5")
            checkable: true
            checked: __formatter.paragraphStyle === DocumentFormatter.H5
            shortcut: qsTr("Ctrl+Shift+5")
            onTriggered: __formatter.paragraphStyle = DocumentFormatter.H5
        }
        
        MenuItem {
            text: qsTr("Title 6")
            checkable: true
            checked: __formatter.paragraphStyle === DocumentFormatter.H6
            shortcut: qsTr("Ctrl+Shift+6")
            onTriggered: __formatter.paragraphStyle = DocumentFormatter.H6
        }
        
        MenuItem {
            text: qsTr("Code")
            checkable: true
            checked: __formatter.paragraphStyle === DocumentFormatter.Code
            shortcut: qsTr("Ctrl+Shift+C")
            onTriggered: __formatter.paragraphStyle = DocumentFormatter.Code
        }
    }
    
    MenuSeparator {}
    
    MenuItem {
        id: bold
        text: qsTr("Bold")
        shortcut: StandardKey.Bold
        checkable: true
        checked: formatMenu.__formatter.bold
        onTriggered: __formatter.bold = !__formatter.bold
    }
    
    MenuItem {
        id: italic
        text: qsTr("Italic")
        shortcut: StandardKey.Italic
        checkable: true
        checked: formatMenu.__formatter.italic
        onTriggered: __formatter.italic = !__formatter.italic
    }
    
    MenuItem {
        id: underline
        text: qsTr("Underline")
        shortcut: StandardKey.Underline
        checkable: true
        checked: formatMenu.__formatter.underline
        onTriggered: __formatter.underline = !__formatter.underline
    }
    
    MenuItem {
        id: strikethrough
        text: qsTr("Strikethrough")
        checkable: true
        checked: formatMenu.__formatter.strikethrough
        onTriggered: __formatter.strikethrough = !__formatter.strikethrough
    }
    
    MenuSeparator {}
    
    MenuItem {
        id: bulletList
        text: qsTr("Bullet List")
        onTriggered: __formatter.unorderedList = !__formatter.unorderedList
    }
    
    MenuItem {
        id: orderedList
        text: qsTr("Ordered List")
        onTriggered: __formatter.orderedList = !__formatter.orderedList
    }
    
    MenuSeparator {}
    
    MenuItem {
        id: indent
        text: qsTr("Indent")
        onTriggered: __formatter.increaseIndentation()
    }
    
    MenuItem {
        id: outdent
        text: qsTr("Outdent")
        onTriggered: __formatter.decreaseIndentation()
    }
    
    MenuSeparator {}
    
    MenuItem {
        id: alignLeft
        text: qsTr("Align Left")
        checkable: true
        checked: formatMenu.__formatter.alignLeft
        onTriggered: __formatter.alignLeft = !__formatter.alignLeft
    }
    
    MenuItem {
        id: alignCenter
        text: qsTr("Center")
        checkable: true
        checked: formatMenu.__formatter.alignCenter
        onTriggered: __formatter.alignCenter = !__formatter.alignCenter
    }
    
    MenuItem {
        id: alignRight
        text: qsTr("Align Right")
        checkable: true
        checked: formatMenu.__formatter.alignRight
        onTriggered: __formatter.alignRight = !__formatter.alignRight
    }
    
    MenuItem {
        id: alignJustify
        text: qsTr("Justify")
        checkable: true
        checked: formatMenu.__formatter.justify
        onTriggered: __formatter.justify = !__formatter.justify
    }
    
}
