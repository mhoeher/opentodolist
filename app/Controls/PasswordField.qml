import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2

import "../Utils" as Utils
import "../Fonts" as Fonts

import "." as C

QQC2.TextField {
    selectByMouse: Utils.AppSettings.selectTextByMouse
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    echoMode: TextInput.Password
    inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
    rightPadding: leftPadding + echoModeToggle.width

    C.ToolButton {
        id: echoModeToggle

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        symbol: {
            switch (parent.echoMode) {
            case TextInput.Password:
                return Fonts.Icons.mdiVisibility;
            default:
                return Fonts.Icons.mdiVisibilityOff;
            }
        }
        onClicked: {
            if (parent.echoMode === TextInput.Password) {
                parent.echoMode = TextInput.Normal;
            } else {
                parent.echoMode = TextInput.Password;
            }
        }
    }
}
