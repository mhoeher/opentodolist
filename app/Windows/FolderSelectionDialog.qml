import QtQuick 2.10
import QtQuick.Layouts 1.3
import Qt.labs.folderlistmodel 2.2

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Fonts"


CenteredDialog {
    id: dialog

    property alias folder: folders.folder

    title: qsTr("Select a Folder")
    standardButtons: C.Dialog.Ok | C.Dialog.Cancel

    header: RowLayout {
        width: parent.width
        C.ToolButton {
            symbol: Icons.mdiKeyboardArrowUp
            onClicked: folders.folder = OTL.Application.cleanPath(
                           folders.folder + "/..")
        }
        C.Label {
            text: OTL.Application.urlToLocalFile(folders.folder)
            width: parent.width
            Layout.fillWidth: true
        }
    }

    ListView {
        implicitWidth: 500
        implicitHeight: 400
        clip: true
        model: FolderListModel {
            id: folders
            showFiles: false
            showDirs: true
            showOnlyReadable: true
            showDotAndDotDot: false

            onFolderChanged: {
                var f = folder.toString();
                f = OTL.Application.urlToLocalFile(f);
                if (f.endsWith("/..")) {
                    f = f.substring(0, f.length - 3);
                    if (f === "") {
                        f = "/";
                    }
                    folder = OTL.Application.localFileToUrl(f);
                }
            }
        }
        delegate: C.ItemDelegate {
            onClicked: folders.folder = OTL.Application.cleanPath(
                           folders.folder + "/" + fileName)
            width: parent.width
            contentItem: RowLayout {
                width: parent.width
                C.Label {
                    text: Icons.mdiFolderOpen
                    font.family: Fonts.icons
                }
                C.Label {
                    text: fileName
                    Layout.fillWidth: true
                }
            }
        }
    }
}
