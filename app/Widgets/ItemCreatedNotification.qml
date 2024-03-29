import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.12

import "../Components"
import "../Controls" as C
import "../Utils"
import "../Windows" as Windows

import OpenTodoList 1.0 as OTL

C.Pane {
    id: root

    function show(item) {
        d.item = item
        d.shown = true
        C.ApplicationWindow.window.itemCreatedNotification = root
        timer.start()
    }

    function trigger() {
        if (d.item) {
            open(d.item)
        }
    }

    signal open(var item)

    Material.background: Material.Green
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

        C.Label {
            text: qsTr("<strong>%1</strong> has been created.").arg(d.itemName)
            Layout.fillWidth: true
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
        }
        C.Button {
            text: qsTr("Open")
            onClicked: root.open(d.item)
        }

        C.Button {
            text: qsTr("Dismiss")
            onClicked: d.shown = false
        }
    }

    QtObject {
        id: d

        property bool shown: false
        property OTL.LibraryItem item: null
        property string itemName: d.item ? Markdown.markdownToPlainText(
                                               d.item.title) : ""
    }

    Timer {
        id: timer

        interval: 5000
        repeat: false
        onTriggered: d.shown = false
    }
}
