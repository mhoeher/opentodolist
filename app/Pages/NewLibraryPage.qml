import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Windows"
import "../Fonts"

NewLibraryPageForm {
    id: page

    signal openPage(var component, var properties)
    signal libraryCreated(OTL.Library library)

    addLocalLibraryDelegate.onClicked: page.openPage(newLocalLibraryPage, {})
    addFolderAsLibraryDelegate.onClicked: page.openPage(newLibraryInFolderPage, {})

    Component {
        id: newLocalLibraryPage

        NewLocalLibraryPage {
            onLibraryCreated: page.libraryCreated(library)
        }
    }

    Component {
        id: newLibraryInFolderPage

        NewLibraryInFolderPage {
            onLibraryCreated: page.libraryCreated(library)
        }
    }

//    signal libraryAvailable(var synchronizer)
//    signal cancelled()

//    property var synchronizer: ({})

//    title: qsTr("Create Library")
//    footer: DialogButtonBox {

//        Button {
//            text: qsTr("Create")
//            enabled: libraryName.inputOkay && directoryEdit.inputOkay
//            onClicked: {
//                var synchronizer = page.synchronizer;
//                if (libraryName.visible) {
//                    synchronizer.name = libraryName.text;
//                }
//                if (!useDefaultLocation.checked) {
//                    synchronizer.localPath = directoryEdit.text;
//                }
//                page.libraryAvailable(synchronizer);
//            }
//            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
//        }

//        Button {
//            text: qsTr("Cancel")
//            onClicked: page.cancelled()
//            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
//        }
//    }

//    ScrollView {
//        id: scrollView
//        anchors.fill: parent

//        Pane {
//            width: scrollView.width

//            Column {
//                id: column
//                width: parent.width

//                Label {
//                    font.bold: true
//                    text: qsTr("Library Name:")
//                    width: parent.width
//                    visible: libraryName.visible
//                }

//                Components.TextField {
//                    id: libraryName
//                    property bool inputOkay: !visible || text !== ""
//                    placeholderText: qsTr("My New Library")
//                    width: parent.width
//                    visible: !synchronizer || synchronizer["name"] === undefined
//                }

//                CheckBox {
//                    id: useDefaultLocation
//                    text: qsTr("Create Library in Default Location")
//                    checked: true
//                    width: parent.width
//                }

//                Label {
//                    text: useDefaultLocation.checked ?
//                              qsTr("The items you add to the library will be stored in the " +
//                                   "default location for libraries.") :
//                              qsTr("Please select a directory into which the library items " +
//                                   "shall be saved. You can also select an existing library " +
//                                   "directory. In this case, the library will be imported.")
//                    width: parent.width
//                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
//                }

//                Item { height: 20; width: 1 }

//                RowLayout {
//                    width: parent.width
//                    visible: !useDefaultLocation.checked

//                    Components.TextField {
//                        id: directoryEdit

//                        property bool inputOkay: selectedNameOk && selectedDirOk
//                        property bool selectedNameOk: {
//                            return useDefaultLocation.checked || text !== "";
//                        }
//                        property bool selectedDirOk: {
//                            return  (libraryName.visible ||
//                                    !OTL.Application.isLibraryDir(
//                                         OTL.Application.localFileToUrl(text))) &&
//                                    (useDefaultLocation.checked ||
//                                     OTL.Application.directoryExists(text));
//                        }

//                        placeholderText: qsTr("Please select a library location")
//                        Layout.fillWidth: true
//                    }

//                    ToolButton {
//                        text: Icons.faFolderOpen
//                        onClicked: {
//                            selectFolder.open();
//                        }
//                    }
//                }
//            }
//        }
//    }

//    FolderSelectionDialog {
//        id: selectFolder
//        onAccepted: directoryEdit.text = OTL.Application.urlToLocalFile(
//                        selectFolder.folder)
//        folder: OTL.Application.librariesLocation
//    }
}
