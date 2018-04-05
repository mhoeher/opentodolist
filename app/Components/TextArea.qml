import QtQuick 2.10
import QtQuick.Controls 2.3

TextArea {
    selectByMouse: {
        switch (Qt.platform.os) {
        case "android":
            return focus;
        default:
            return true;
        }
    }
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
