import QtQuick 2.12
import QtQuick.Controls 2.5

import "." as Controls
import "../Utils" as Utils

Controls.Label {
    property int level: 1

    font.pointSize: Utils.AppSettings.effectiveFontSize + 7 - level
    font.bold: true
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    bottomPadding: Utils.AppSettings.smallSpace + Utils.AppSettings.mediumSpace * (1.0 - level / 6.0)
    topPadding: bottomPadding
}
