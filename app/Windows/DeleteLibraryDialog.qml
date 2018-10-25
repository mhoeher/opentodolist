import QtQuick 2.0

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Utils"


CenteredDialog {
    id: dialog

    function deleteLibrary(library) {
        d.library = library;
        dialog.open();
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
    title: qsTr("Delete Library?")
    width: idealDialogWidth

    onAccepted: {
        var library = d.library;
        OTL.Application.deleteLibrary(library);
    }

    QtObject {
        id: d

        property OTL.Library library

        readonly property string textDefaultLocation: {
            return qsTr("Do you really want to remove the library " +
                        "<strong>%1</strong> from  the " +
                        "application? <em>This will remove any files " +
                        "belonging to the library.</em>"
                        );
        }
        readonly property string textNonDefaultLocation: {
            return qsTr("Do you really want to remove the library " +
                        "<strong>%1</strong> from the " +
                        "application? Note that the files inside the " +
                        "library will not be removed, so " +
                        "you can restore the library later on."
                        );
        }
    }

    Label {
        width: parent.width
        text: {
            if (d.library) {
                if (d.library.isInDefaultLocation) {
                    var msg = d.textDefaultLocation;
                } else {
                    msg = d.textNonDefaultLocation;
                }
                var title = d.library.name;
                return msg.arg(title);
            }
            return "";
        }
    }
}
