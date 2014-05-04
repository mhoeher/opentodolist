import QtQuick 2.0

TextEdit {
    id: edit

    selectByMouse: {
        switch ( Qt.platform.os ) {
        case "android": return false;
        case "ios": return false;
        case "blackberry": return false;
        default: return true;
        }
    }
}
