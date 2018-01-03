import QtQuick 2.9

import OpenTodoList 1.0

Item {
    id: dialog

    property alias fileUrl: d.currentFile

    function open() {
        d.currentFile = "";
        if (!d.androidDialog) {
            var androidDialog = component.createObject(d);
            d.androidDialog = androidDialog;
        }
        d.androidDialog.open();
    }

    signal accepted()
    signal rejected()

    QtObject {
        id: d

        property url currentFile
        property AndroidFileDialog androidDialog: null

        function openFinished(url) {
            if (url) {
                d.currentFile = url;
                dialog.accepted();
            } else {
                dialog.rejected();
            }
        }
    }

    Component {
        id: component

        AndroidFileDialog {
            type: AndroidFileDialog.SelectImage
            receiver: d
        }
    }

}
