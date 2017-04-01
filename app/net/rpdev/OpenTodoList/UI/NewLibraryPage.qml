import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Page {
    id: page

    property var stack: null

    signal closed()

    signal newLibrary(string name, url location)

    function cancel() {
        closed();
    }

    footer: ToolBar {
        padding: Globals.defaultMargin

        Button {
            text: qsTr("Create")
            anchors.right: parent.right
            enabled: {
                var result = libraryName.acceptableInput;
                if (!useDefaultLocation.checked) {
                    result = result && App.directoryExists(directoryEdit.text);
                }
                return result;
            }
            onClicked: {
                if (useDefaultLocation.checked) {
                    page.newLibrary(libraryName.text, null);
                } else {
                    var location = App.localFileToUrl(directoryEdit.text);
                    page.newLibrary(libraryName.text, location);
                }
                page.stack.pop();
            }
        }
    }

    QtObject {
        id: d

        property FileBrowser fileBrowser: null
    }

    Flickable {

        anchors {
            fill: parent
            margins: Globals.defaultMargin
        }
        contentHeight: contentItem.childrenRect.height
        ScrollIndicator.vertical: ScrollIndicator {}

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }

            TextField {
                id: libraryName
                placeholderText: qsTr("Library Name")
                width: parent.width
                validator: RegExpValidator {
                    regExp: /^.+$/
                }
            }

            CheckBox {
                id: useDefaultLocation
                text: qsTr("Create Library in Default Location")
                checked: true
            }

            Text {
                text: useDefaultLocation.checked ?
                          qsTr("The items you add to the library will be stored in the " +
                               "default location for libraries.") :
                          qsTr("Please select a directory into which the library items " +
                               "shall be saved. You can also select an existing library " +
                               "directory. In this case, the library will be imported.")
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Item { height: Globals.defaultMargin; width: 1 }

            RowLayout {
                width: parent.width
                visible: !useDefaultLocation.checked

                TextField {
                    id: directoryEdit
                    placeholderText: qsTr("Please select a library location")
                    Layout.fillWidth: true
                }

                RoundButton {
                    font.family: Fonts.symbols.family
                    text: Fonts.symbols.faFolderOpenO
                    onClicked: {
                        if (d.fileBrowser === null) {
                            d.fileBrowser = selectFolderComponent.createObject();
                        }
                        d.fileBrowser.open();
                    }
                }
            }
        }
    }

    Component {
        id: selectFolderComponent
        FileBrowser {
            selectFolder: true
            selectExisting: true
            stack: page.stack

            onAccepted: {
                directoryEdit.text = App.urlToLocalFile(fileUrl);
                directoryEdit.focus = true;
            }
            onRejected: directoryEdit.focus = true
        }
    }
}
