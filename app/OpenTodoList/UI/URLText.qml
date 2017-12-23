import QtQuick 2.9
import QtQuick.Controls 2.2

Label {
    id: label

    property string plainText: ""

    text: {
        var re = /(\w+:\/\/[^\s]+)/g;
        return (plainText
                .replace(/</g, "&lt;")
                .replace(/>/g, "&gt;")
                .replace(re, "<a href='$1'>$1</a>"));
    }

    onLinkActivated: Qt.openUrlExternally(link)
}
