import QtQuick 2.0

import OpenTodoList 1.0 as OTL

Item {
    id: dialog

    signal accepted

    function openUrl(url) {
        // Open the URL externally:
        shareUtils.openLink(url)
    }

    function finish() {
        dialog.accepted()
    }

    OTL.ShareUtils {
        id: shareUtils
    }
}
