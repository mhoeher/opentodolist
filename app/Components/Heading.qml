import QtQuick 2.12
import QtQuick.Controls 2.5

import "." as Controls

Controls.Label {
    property int level: 1

    FontMetrics {
        id: fontMetrics
    }

    font.pixelSize: fontMetrics.font.pixelSize * 2.0 * (1.0 - level / 6.0)
    font.bold: true
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
