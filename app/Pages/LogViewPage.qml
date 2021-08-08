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

    property var pageActions: ([
                                   copyLogAction,
                                   jumpToPageStart,
                                   jumpToPageEnd
                               ])

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
                    var map = {
                        "Debug": Icons.mdiInbox,
                        "Warning": Icons.mdiFeedback,
                        "Error": Icons.mdiError,
                        "Download": Icons.mdiDownload,
                        "Upload": Icons.mdiUpload,
                        "LocalMkDir": Icons.mdiFolder,
                        "RemoteMkDir": Icons.mdiDriveFolderUpload,
                        "LocalDelete": Icons.mdiDelete,
                        "RemoteDelete": Icons.mdiDeleteSweep
                    };
                    return map[modelData.type] || Icons.mdiHelpOutline;
                }
                Layout.alignment: Qt.AlignTop

                C.ToolTip {
                    text: modelData.type
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
