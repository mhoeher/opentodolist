import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    property var synchronizer: ({})

    signal cancelled()
    signal libraryAvailable(var synchronizer)

    onSynchronizerChanged: {
        dav.url = synchronizer.url;
        dav.username = synchronizer.username;
        dav.password = synchronizer.password;
        dav.disableCertificateCheck = synchronizer.disableCertificateCheck;
        dav.serverType = synchronizer.serverType;
        dav.findExistingLibraries();
    }
    footer: DialogButtonBox {
        Button {
            text: qsTr("OK")
            onClicked: {
                var synchronizer = page.synchronizer;
                synchronizer.uid = d.uid;
                synchronizer.path = d.path;
                if (createNewLibrary.checked) {
                    synchronizer.name = newLibraryName.text;
                } else {
                    synchronizer.name = d.name;
                }
                page.libraryAvailable(synchronizer);
            }
            enabled: !createNewLibrary.checked || newLibraryName.text != ""
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }

        Button {
            text: qsTr("Cancel")
            onClicked: page.cancelled()
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    QtObject {
        id: d

        property string name: ""
        property string path: ""
        property string uid: ""
    }

    ButtonGroup { id: libraryRadioGroup }

    WebDAVSynchronizer { id: dav }

    ScrollView {
        anchors.fill: parent

        Flickable {
            id: flickable
            anchors.fill: parent

            ColumnLayout {
                width: flickable.width - 2 * Globals.defaultMargin
                x: Globals.defaultMargin
                y: Globals.defaultMargin

                RadioButton {
                    id: createNewLibrary
                    text: qsTr("Create new library on server")
                    checked: true
                    Layout.fillWidth: true
                    onCheckedChanged: {
                        if (checked) {
                            d.path = "";
                            d.uid = "";
                        }
                    }
                }

                TextField {
                    id: newLibraryName
                    x: Globals.defaultMargin
                    visible: createNewLibrary.checked
                    Layout.fillWidth: true
                }

                Text {
                    text: qsTr("Searching for existing libraries...")
                    Layout.alignment: Qt.AlignHCenter
                    visible: dav.findingLibraries
                }

                BusyIndicator {
                    Layout.alignment: Qt.AlignHCenter
                    visible: dav.findingLibraries
                }

                Repeater {
                    model: dav.existingLibraries
                    delegate: RadioButton {
                        text: modelData.name
                        onCheckedChanged: {
                            if (checked) {
                                d.name = modelData.name;
                                d.path = modelData.path;
                                d.uid = modelData.uid;
                            }
                        }
                    }
                }
            }
        }
    }
}
