import QtQuick 2.10
import QtQuick.Layouts 1.3
import Qt.labs.folderlistmodel 2.2

import OpenTodoList 1.0 as OTL

import "../Fonts"
import "../Components"
import "../Controls" as C


CenteredDialog {
    id: dialog

    property url fileUrl: ""
    property alias nameFilters: files.nameFilters

    title: qsTr("Select a File")
    standardButtons: fileUrl != "" ? (C.Dialog.Ok | C.Dialog.Cancel) : C.Dialog.Cancel

    header: RowLayout {
        width: parent.width
        C.ToolButton {
            symbol: Icons.mdiKeyboardArrowUp
            onClicked: files.folder = OTL.Application.cleanPath(
                           files.folder + "/..")
        }
        C.Label {
            text: OTL.Application.urlToLocalFile(dialog.fileUrl)
            Layout.fillWidth: true
        }
    }

    ListView {
        implicitWidth: 300
        implicitHeight: 400
        clip: true


        model: FolderListModel {
            id: files
            showFiles: true
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
            width: parent.width
            onClicked: {
                if (fileIsDir) {
                    files.folder = OTL.Application.cleanPath(files.folder +
                                                             "/" + fileName);
                } else {
                    dialog.fileUrl = fileURL;
                }
            }
            checked: dialog.fileUrl === fileURL
            contentItem: RowLayout {
                width: parent.width

                C.Label {
                    font.family: Fonts.icons
                    text: fileIsDir ? Icons.mdiFolderOpen :
                                      Icons.mdiTextSnippet
                }
                C.Label {
                    Layout.fillWidth: true
                    text: fileName
                }
            }
        }
    }
}
