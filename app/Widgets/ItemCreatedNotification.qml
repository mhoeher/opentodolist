import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import "../Components"
import "../Utils"

import OpenTodoList 1.0 as OTL

Pane {
    id: root

    function show(item) {
        d.item = item;
        d.shown = true;
        ApplicationWindow.window.itemCreatedNotification = root;
        timer.start();
    }

    function trigger() {
        if (d.item) {
            open(d.item);
        }
    }

    signal open(OTL.LibraryItem item)

    Material.background:  Material.Green
    z: 11
    anchors {
        left: parent.left
        right: parent.right
    }
    y: root.parent.height
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

    RowLayout {
        id: layout
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }

        Label {
            text: qsTr("<strong>%1</strong> has been created.").arg(d.itemName)
            Layout.fillWidth: true
            elide: Text.ElideRight
        }
        Button {
            text: qsTr("Open")
            onClicked: root.open(d.item)
        }

        Button {
            text: qsTr("Dismiss")
            onClicked: d.shown = false
        }
    }

    QtObject {
        id: d

        property bool shown: false
        property OTL.LibraryItem item: null
        property string itemName: d.item ? Markdown.markdownToPlainText(d.item.title) : ""
    }

    Timer {
        id: timer

        interval: 5000
        repeat: false
        onTriggered: d.shown = false
    }
}
