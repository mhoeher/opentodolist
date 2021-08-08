import QtQuick 2.0
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Utils" as Utils
import "../Controls" as C

C.ScrollView {
    property OTL.TopLevelItem item: null

    Material.background: Utils.Colors.materialItemBackgroundColor(item)
}
