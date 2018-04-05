import QtQuick 2.10
import QtQuick.Controls 2.3

Label {
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

    onLinkActivated: {
        if (/^(http|https|file):\/\/.*$/.exec(link)) {
            Qt.openUrlExternally(link)
        }
    }
}
