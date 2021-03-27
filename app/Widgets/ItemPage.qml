import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import OpenTodoList 1.0 as OTL

import "../Utils" as Utils

Page {
    property OTL.TopLevelItem topLevelItem: null

    property bool syncRunning: {
        return library &&
                OTL.Application.directoriesWithRunningSync.indexOf(
                    library.directory) >= 0;
    }
    property int syncProgress: {
        let result = -1;
        if (library) {
            result = OTL.Application.syncProgress[library.directory];
            if (result === undefined) {
                result = -1;
            }
        }
        return result;
    }

    Material.background: Utils.Colors.materialItemBackgroundColor(topLevelItem)
}
