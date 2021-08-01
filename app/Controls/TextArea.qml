import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2

import "../Utils" as Utils

QQC2.TextArea {
    selectByMouse: Utils.AppSettings.selectTextByMouse
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
