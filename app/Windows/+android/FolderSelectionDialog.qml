import QtQuick 2.10
import QtQuick.Layouts 1.3
import Qt.labs.folderlistmodel 2.2

import "../Controls" as C
import "../Fonts" as Fonts
import "../Utils" as Utils
import "."

import OpenTodoList 1.0 as OTL

CenteredDialog {
    id: dialog

    property url folder
    property url selectedFolder: folderListModel.folder

    standardButtons: C.DialogButtonBox.Ok | C.DialogButtonBox.Cancel
    width: dialog.idealDialogWidth
    height: dialog.idealDialogHeight

    header: C.ToolBar {
        leftPadding: Utils.AppSettings.mediumSpace
        rightPadding: Utils.AppSettings.mediumSpace

        RowLayout {
            width: parent.width

            C.ToolButton {
                symbol: Fonts.Icons.mdiArrowUpward
                visible: {
                    let parentFolder = OTL.Application.getParentDirectory(
                            folderListModel.folder)
                    return OTL.Application.canListPath(parentFolder)
                }
                onClicked: {
                    if (visible) {
                        let parentFolder = OTL.Application.getParentDirectory(
                                folderListModel.folder)
                        folderListModel.folder = parentFolder
                    }
                }
            }

            C.Label {
                text: OTL.Application.urlToLocalFile(dialog.selectedFolder)
                elide: Text.ElideMiddle

                Layout.fillWidth: true
            }
        }
    }

    C.ScrollView {
        id: scrollView

        width: dialog.contentItem.width
        height: dialog.contentItem.height
        clip: true

        ListView {
            width: scrollView.availableWidth
            height: scrollView.availableHeight

            model: FolderListModel {
                id: folderListModel

                folder: dialog.folder
                showFiles: false
                showDirs: true
            }
            delegate: C.ItemDelegate {
                text: fileName
                onClicked: {
                    let newFolder = OTL.Application.localFileToUrl(
                            OTL.Application.urlToLocalFile(
                                folderListModel.folder) + "/" + fileName)
                    if (OTL.Application.canListPath(newFolder)) {
                        folderListModel.folder = newFolder
                    }
                }
            }
        }
    }
}
