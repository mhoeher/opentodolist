import QtQuick 2.10
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Fonts"
import "../Utils" as Utils

Page {
    id: page

    title: qsTr("Synchronization Log")

    property alias log: view.model

    property var pageActions: ([
                                   copyLogAction,
                                   jumpToPageStart,
                                   jumpToPageEnd
                               ])

    Action {
        id: copyLogAction
        text: qsTr("Copy Log")
        onTriggered: OTL.Application.copyToClipboard(JSON.stringify(page.log))
    }

    Action {
        id: jumpToPageStart
        text: qsTr("Scroll to Top")
        onTriggered: view.positionViewAtBeginning()
    }

    Action {
        id: jumpToPageEnd
        text: qsTr("Scroll to Bottom")
        onTriggered: view.positionViewAtEnd()
    }

    ListView {
        id: view

        anchors.fill: parent
        ScrollBar.vertical: ScrollBar {}
        spacing: Utils.AppSettings.mediumSpace

        delegate: RowLayout {
            width: view.width
            ToolButton {
                symbol: {
                    var map = {
                        "Debug": Icons.faInfo,
                        "Warning": Icons.faExclamation,
                        "Error": Icons.faTimesCircle,
                        "Download": Icons.faDownload,
                        "Upload": Icons.faUpload,
                        "LocalMkDir": Icons.faFolder,
                        "RemoteMkDir": Icons.faFolderOpen,
                        "LocalDelete": Icons.faTrash,
                        "RemoteDelete": Icons.faTrashAlt
                    };
                    return map[modelData.type] || Icons.faQuestionCircle;
                }
                Layout.alignment: Qt.AlignTop
                ToolTip.text: modelData.type
                ToolTip.visible: hovered
                ToolTip.delay: 500
            }
            Label {
                text: modelData.time
                Layout.maximumWidth: view.width / 4
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                Layout.alignment: Qt.AlignTop
            }
            Label {
                Layout.fillWidth: true
                text: modelData.message
                Layout.alignment: Qt.AlignTop
            }
        }
    }
}
