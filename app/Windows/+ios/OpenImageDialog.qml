import QtQuick 2.10
import QtQuick.Dialogs 1.2

Item {
    id: dialog

    property alias fileUrl: fileDialog.fileUrl

    function open() {
        fileDialog.open()
    }

    signal accepted()
    signal rejected()

    FileDialog {
        id: fileDialog

        title: qsTr("Select Image")
        folder: shortcuts.pictures

        onAccepted: dialog.accepted();
        onRejected: dialog.rejected()
    }
}
