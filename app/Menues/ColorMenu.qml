import QtQuick 2.10
import "../Components" as Components
import "../Controls" as C

import OpenTodoList 1.0 as OTL

C.Menu {
    id: menu

    property OTL.TopLevelItem item: null

    property OTL.TopLevelItem __defaultItem: OTL.TopLevelItem {}
    property OTL.TopLevelItem __effectiveItem: item || __defaultItem

    title: qsTr("Color")
    modal: true

    C.MenuItem {
        text: qsTr("White")
        checkable: true
        checked: __effectiveItem.color === OTL.TopLevelItem.White
        onTriggered: __effectiveItem.color = OTL.TopLevelItem.White
    }

    C.MenuItem {
        text: qsTr("Red")
        checkable: true
        checked: __effectiveItem.color === OTL.TopLevelItem.Red
        onTriggered: __effectiveItem.color = OTL.TopLevelItem.Red
    }

    C.MenuItem {
        text: qsTr("Green")
        checkable: true
        checked: __effectiveItem.color === OTL.TopLevelItem.Green
        onTriggered: __effectiveItem.color = OTL.TopLevelItem.Green
    }

    C.MenuItem {
        text: qsTr("Blue")
        checkable: true
        checked: __effectiveItem.color === OTL.TopLevelItem.Blue
        onTriggered: __effectiveItem.color = OTL.TopLevelItem.Blue
    }

    C.MenuItem {
        text: qsTr("Yellow")
        checkable: true
        checked: __effectiveItem.color === OTL.TopLevelItem.Yellow
        onTriggered: __effectiveItem.color = OTL.TopLevelItem.Yellow
    }

    C.MenuItem {
        text: qsTr("Orange")
        checkable: true
        checked: __effectiveItem.color === OTL.TopLevelItem.Orange
        onTriggered: __effectiveItem.color = OTL.TopLevelItem.Orange
    }

    C.MenuItem {
        text: qsTr("Lilac")
        checkable: true
        checked: __effectiveItem.color === OTL.TopLevelItem.Lilac
        onTriggered: __effectiveItem.color = OTL.TopLevelItem.Lilac
    }
}
