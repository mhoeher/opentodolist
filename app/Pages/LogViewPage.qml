import QtQuick 2.10
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Utils" as Utils
import "../Controls" as C

C.Page {
    id: page

    title: qsTr("Synchronization Log")

    property alias log: view.model

    property var pageActions: ([copyLogAction, jumpToPageStart, jumpToPageEnd])

    C.Action {
        id: copyLogAction
        text: qsTr("Copy Log")
        onTriggered: OTL.Application.copyToClipboard(JSON.stringify(page.log))
    }

    C.Action {
        id: jumpToPageStart
        text: qsTr("Scroll to Top")
        onTriggered: view.positionViewAtBeginning()
    }

    C.Action {
        id: jumpToPageEnd
        text: qsTr("Scroll to Bottom")
        onTriggered: view.positionViewAtEnd()
    }

    ListView {
        id: view

        anchors.fill: parent
        C.ScrollBar.vertical: C.ScrollBar {}
        spacing: Utils.AppSettings.mediumSpace

        delegate: RowLayout {
            width: view.width
            C.ToolButton {
                symbol: {
                    switch (modelData.type) {
                    case OTL.Synchronizer.Debug:
                        return Icons.mdiInbox
                    case OTL.Synchronizer.Warning:
                        return Icons.mdiFeedback
                    case OTL.Synchronizer.Error:
                        return Icons.mdiError
                    case OTL.Synchronizer.Download:
                        return Icons.mdiDownload
                    case OTL.Synchronizer.Upload:
                        return Icons.mdiUpload
                    case OTL.Synchronizer.LocalMkDir:
                        return Icons.mdiFolder
                    case OTL.Synchronizer.RemoteMkDir:
                        return Icons.mdiDriveFolderUpload
                    case OTL.Synchronizer.LocalDelete:
                        return Icons.mdiDelete
                    case OTL.Synchronizer.RemoteDelete:
                        return Icons.mdiDeleteSweep
                    default:
                        return Icons.mdiHelpOutline
                    }
                }
                Layout.alignment: Qt.AlignTop

                C.ToolTip {
                    text: {
                        switch (modelData.type) {
                        case OTL.Synchronizer.Debug:
                            return qsTr("Debugging information")
                        case OTL.Synchronizer.Warning:
                            return qsTr("Warning")
                        case OTL.Synchronizer.Error:
                            return qsTr("Error")
                        case OTL.Synchronizer.Download:
                            return qsTr("Download")
                        case OTL.Synchronizer.Upload:
                            return qsTr("Upload")
                        case OTL.Synchronizer.LocalMkDir:
                            return qsTr("Create local folder")
                        case OTL.Synchronizer.RemoteMkDir:
                            return qsTr("Create remote folder")
                        case OTL.Synchronizer.LocalDelete:
                            return qsTr("Deleting locally")
                        case OTL.Synchronizer.RemoteDelete:
                            return qsTr("Deleting remotely")
                        default:
                            return qsTr("Unknown log message type")
                        }
                    }
                }
            }
            C.Label {
                text: modelData.time
                Layout.maximumWidth: view.width / 4
                Layout.alignment: Qt.AlignTop
            }
            C.Label {
                Layout.fillWidth: true
                text: modelData.message
                Layout.alignment: Qt.AlignTop
            }
        }
    }
}
