import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1

import OpenTodoList 1.0
import OpenTodoList.UI 1.0

Page {
    id: page

    signal libraryAvailable(var synchronizer)
    signal cancelled()

    property var synchronizer: ({})

    footer: DialogButtonBox {

        Button {
            text: qsTr("Create")
            enabled: libraryName.inputOkay && directoryEdit.inputOkay
            onClicked: {
                var synchronizer = page.synchronizer;
                if (libraryName.visible) {
                    synchronizer.name = libraryName.text;
                }
                if (!useDefaultLocation.checked) {
                    synchronizer.localPath = directoryEdit.text;
                }
                page.libraryAvailable(synchronizer);
            }
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }

        Button {
            text: qsTr("Cancel")
            onClicked: page.cancelled()
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent

        Column {
            width: scrollView.width - Globals.defaultMargin
            x: Globals.defaultMargin / 2
            y: x

            Label {
                text: qsTr("Library Name:")
                width: parent.width
                wrapMode: Text.WordWrap
                visible: libraryName.visible
            }

            TextField {
                id: libraryName
                property bool inputOkay: !visible || acceptableInput
                placeholderText: qsTr("My New Library")
                width: parent.width
                validator: RegExpValidator {
                    regExp: /^.+$/
                }
                visible: !synchronizer || synchronizer["name"] === undefined
                selectByMouse: Globals.selectTextWithMouse
            }

            CheckBox {
                id: useDefaultLocation
                text: qsTr("Create Library in Default Location")
                checked: true
            }

            Label {
                text: useDefaultLocation.checked ?
                          qsTr("The items you add to the library will be stored in the " +
                               "default location for libraries.") :
                          qsTr("Please select a directory into which the library items " +
                               "shall be saved. You can also select an existing library " +
                               "directory. In this case, the library will be imported.")
                width: parent.width
                wrapMode: Text.WordWrap
            }

            Item { height: Globals.defaultMargin; width: 1 }

            RowLayout {
                width: parent.width
                visible: !useDefaultLocation.checked

                TextField {
                    id: directoryEdit

                    property bool inputOkay: selectedNameOk && selectedDirOk
                    property bool selectedNameOk: {
                        return useDefaultLocation.checked || text !== "";
                    }
                    property bool selectedDirOk: {
                        return  (libraryName.visible ||
                                !App.isLibraryDir(App.localFileToUrl(text))) &&
                                (useDefaultLocation.checked ||
                                 App.directoryExists(text));
                    }

                    placeholderText: qsTr("Please select a library location")
                    Layout.fillWidth: true
                }

                RoundButton {
                    font.family: Fonts.symbols.family
                    text: Fonts.symbols.faFolderOpenO
                    onClicked: {
                        selectFolder.open();
                    }
                }
            }
        }
    }

    FolderSelectionDialog {
        id: selectFolder
        onAccepted: directoryEdit.text = App.urlToLocalFile(selectFolder.folder)
        folder: App.librariesLocation
    }
}
