import QtQuick 2.0
import QtWebView 1.1 as WebView

import ".." as W
import "../../Controls" as C

W.CenteredDialog {
    id: dialog

    modal: true
    width: C.ApplicationWindow.contentItem.width * 0.9
    height: C.ApplicationWindow.contentItem.height * 0.9
    title: {
        if (webView.loading) {
            return qsTr("Loading...") + " " + webView.loadProgress + "%"
        } else {
            return webView.title
        }
    }

    standardButtons: C.Dialog.Close

    function openUrl(url) {
        // Open the URL in the webview:
        webView.url = url
        open()
    }

    function finish() {
        dialog.accepted()
    }

    WebView.WebView {
        id: webView

        anchors.fill: parent
    }
}
