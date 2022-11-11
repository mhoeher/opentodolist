import QtQuick 2.12

Item {
    property alias settingsShortcut: settingsShortcut
    property alias newShortcut: newShortcut
    property alias quitShortcut: quitShortcut
    property alias closeShortcut: closeShortcut
    property alias findShortcut: findShortcut
    property alias undoShortcut: undoShortcut
    property alias goBackShortcut: goBackShortcut
    property alias openShortcut: openShortcut

    Shortcut {
        id: settingsShortcut
        sequence: qsTr("Ctrl+,")
    }

    Shortcut {
        id: newShortcut
        sequence: StandardKey.New
    }

    Shortcut {
        id: quitShortcut
        sequence: StandardKey.Quit
    }

    Shortcut {
        id: closeShortcut
        sequence: [StandardKey.Close]
    }

    Shortcut {
        id: findShortcut
        sequence: StandardKey.Find
    }

    Shortcut {
        id: undoShortcut
        sequence: StandardKey.Undo
    }

    Shortcut {
        id: goBackShortcut
        sequences: [StandardKey.Back, "Esc", "Back"]
        enabled: Qt.platform.os !== "android" && Qt.platform.os !== "ios"
    }

    Shortcut {
        id: openShortcut
        sequence: StandardKey.Open
    }
}
