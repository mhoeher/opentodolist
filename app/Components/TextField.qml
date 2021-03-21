import QtQuick 2.10
import QtQuick.Controls 2.3

import "../Utils" as Utils

TextField {
    selectByMouse: Utils.AppSettings.selectTextByMouse
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
