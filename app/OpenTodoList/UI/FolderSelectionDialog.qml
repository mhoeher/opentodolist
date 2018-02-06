import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.folderlistmodel 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

CenteredDialog {
    id: dialog

    property alias folder: folders.folder

    title: qsTr("Select a Folder")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    header: RowLayout {
        Symbol {
            symbol: Fonts.symbols.faArrowUp
            onClicked: folders.folder = App.cleanPath(folders.folder + "/..")
        }
    }

    ListView {
        implicitWidth: 300
        implicitHeight: 400
        header: Text {
            text: qsTr("Selected folder: ") + "<strong>" +
                  App.urlToLocalFile(folders.folder) + "</strong>"
            width: parent.width
            clip: true
            wrapMode: Text.Word
        }
        clip: true


        model: FolderListModel {
            id: folders
            showFiles: false
            showDirs: true
            showOnlyReadable: true
            showDotAndDotDot: false

            onFolderChanged: {
                var f = folder.toString();
                f = App.urlToLocalFile(f);
                if (f.endsWith("/..")) {
                    f = f.substring(0, f.length - 3);
                    if (f === "") {
                        f = "/";
                    }
                    folder = App.localFileToUrl(f);
                }
            }
        }
        delegate: MouseArea {
            width: parent.width
            height: childrenRect.height + Globals.defaultMargin
            onClicked: folders.folder = App.cleanPath(folders.folder + "/" + fileName)

            RowLayout {
                y: Globals.defaultMargin / 2
                x: Globals.defaultMargin / 2
                width: parent.width - Globals.defaultMargin

                Symbol {
                    symbol: Fonts.symbols.faFolderOpenO
                }
                Label {
                    Layout.fillWidth: true
                    text: fileName
                    wrapMode: Text.Word
                }
            }
        }
    }

}
