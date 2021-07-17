import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2

import "../Utils" as Utils

QQC2.ToolTip {
    delay: Utils.AppSettings.tooltipDelay
    timeout: Utils.AppSettings.tooltipTimeout
    visible: {
        if (parent && parent.hovered !== undefined) {
            return parent.hovered;
        }
        return false;
    }
}
