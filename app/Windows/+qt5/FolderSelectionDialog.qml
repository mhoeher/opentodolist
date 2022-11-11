import QtQuick 2.10
import QtQuick.Layouts 1.3
import Qt.labs.folderlistmodel 2.2
import QtQuick.Dialogs 1.2 as Dialogs

import OpenTodoList 1.0 as OTL

Dialogs.FileDialog {
    id: dialog

    property alias selectedFolder: selectedFile

    selectFolder: true
}
