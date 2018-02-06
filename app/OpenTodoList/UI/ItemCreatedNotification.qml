import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0


Rectangle {
    id: root

    function show(item) {
        d.item = item;
        d.shown = true;
        Globals.itemCreatedNotification = root;
        timer.start();
    }

    function trigger() {
        if (d.item) {
            open(d.item);
        }
    }

    signal open(BasicItem item)

    anchors {
        left: parent.left
        right: parent.right
    }
    height: layout.height + Globals.defaultMargin
    color: Colors.infoBackground
    y: parent.height

    states: State {
        name: "shown"
        when: d.shown

        PropertyChanges {
            target: root
            y: root.parent.height - root.height
        }
    }

    transitions: Transition {
        from: ""
        to: "shown"
        reversible: true

        SmoothedAnimation {
            properties: "y"
        }
    }

    QtObject {
        id: d

        property bool shown: false
        property BasicItem item: null
        property string itemName: item ? item.displayTitle : ""
    }

    Timer {
        id: timer

        interval: 5000
        repeat: false
        onTriggered: d.shown = false
    }

    RowLayout {
        id: layout
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
            margins: Globals.defaultMargin
        }

        Label {
            text: qsTr("<strong>%1</strong> has been created.").arg(
                      d.itemName)
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
        Button {
            text: qsTr("Open")
            onClicked: root.open(d.item)
        }
    }
}
