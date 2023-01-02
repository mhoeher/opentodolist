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
        sequences: [StandardKey.New]
    }

    Shortcut {
        id: quitShortcut
        sequences: [StandardKey.Quit]
    }

    Shortcut {
        id: closeShortcut
        sequences: [StandardKey.Close]
    }

    Shortcut {
        id: findShortcut
        sequences: [StandardKey.Find]
    }

    Shortcut {
        id: undoShortcut
        sequences: [StandardKey.Undo]
    }

    Shortcut {
        id: goBackShortcut
        sequences: [StandardKey.Back, "Esc", "Back"]
        enabled: Qt.platform.os !== "android" && Qt.platform.os !== "ios"
    }

    Shortcut {
        id: openShortcut
        sequences: [StandardKey.Open]
    }
}
