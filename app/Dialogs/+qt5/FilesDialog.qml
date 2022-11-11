import QtQuick 2.15
import QtQuick.Dialogs 1.2 as Dialogs

Dialogs.FileDialog {
    selectMultiple: true
    property alias selectedFile: fileUrl
    property alias selectedFiles: fileUrls
}
