import QtQuick 2.0

Item {
    id: dialog

    signal accepted

    function openUrl(url) {
        // Open the URL externally:
        Qt.openUrlExternally(url)
    }

    function finish() {
        dialog.accepted()
    }
}
