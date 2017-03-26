import QtQuick 2.0
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Menu {
    id: menu

    property TopLevelItem item: null

    property TopLevelItem __defaultItem: TopLevelItem {}
    property TopLevelItem __effectiveItem: item || __defaultItem

    title: qsTr("&Color")
    visible: item

    MenuItem {
        text: qsTr("&White")
        checkable: true
        checked: __effectiveItem.color === TopLevelItem.White
        onTriggered: __effectiveItem.color = TopLevelItem.White
    }

    MenuItem {
        text: qsTr("&Red")
        checkable: true
        checked: __effectiveItem.color === TopLevelItem.Red
        onTriggered: __effectiveItem.color = TopLevelItem.Red
    }

    MenuItem {
        text: qsTr("&Green")
        checkable: true
        checked: __effectiveItem.color === TopLevelItem.Green
        onTriggered: __effectiveItem.color = TopLevelItem.Green
    }

    MenuItem {
        text: qsTr("&Blue")
        checkable: true
        checked: __effectiveItem.color === TopLevelItem.Blue
        onTriggered: __effectiveItem.color = TopLevelItem.Blue
    }

    MenuItem {
        text: qsTr("&Yellow")
        checkable: true
        checked: __effectiveItem.color === TopLevelItem.Yellow
        onTriggered: __effectiveItem.color = TopLevelItem.Yellow
    }

    MenuItem {
        text: qsTr("&Orange")
        checkable: true
        checked: __effectiveItem.color === TopLevelItem.Orange
        onTriggered: __effectiveItem.color = TopLevelItem.Orange
    }

    MenuItem {
        text: qsTr("&Lilac")
        checkable: true
        checked: __effectiveItem.color === TopLevelItem.Lilac
        onTriggered: __effectiveItem.color = TopLevelItem.Lilac
    }
}
