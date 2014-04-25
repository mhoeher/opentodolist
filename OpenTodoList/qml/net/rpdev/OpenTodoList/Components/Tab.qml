import QtQuick 2.0

Item {
    id: tab

    property string name: qsTr( "Tab" )
    property HeaderBarItem headerBarItem: HeaderBarItem {
        id: headerBarItem
        itemText: tab.name
    }
}
