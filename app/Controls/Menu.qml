import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2

QQC2.Menu {
    width: {
        var result = implicitWidth
        var padding = 0
        for (var i = 0; i < count; ++i) {
            var item = itemAt(i)
            result = Math.max(item.contentItem.implicitWidth, result)
            padding = Math.max(item.padding, padding)
        }
        return result + padding * 2
    }
}
