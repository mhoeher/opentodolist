import QtQuick 2.10
import Qt.labs.platform 1.0

Item {
    id: dialog

    property alias fileUrl: fileDialog.currentFile

    function open() {
        fileDialog.open()
    }

    signal accepted()
    signal rejected()

    FileDialog {
        id: fileDialog

        title: qsTr("Select Image")
        nameFilters: ["Image Files (*.png *.bmp *.jpg *.jpeg *.gif)"]
        onAccepted: dialog.accepted()
        onRejected: dialog.rejected()
    }
}
