import QtQuick 2.4
import QtQuick.Controls 2.12

import "../Utils" as Utils

SettingsPageForm {
    property StackView stack: null

    width: stack.width
    height: stack.height

    onClose: {
        stack.pop();
    }

    themeEdit.currentIndex: {
        var currentTheme = Utils.Colors.theme;
        var themeName = Utils.Colors.themeIdsToNameMap[Utils.Colors.theme];
        if (themeName) {
            var idx = Utils.Colors.themeNames.indexOf(themeName);
            if (idx >= 0) {
                return idx;
            }
        }
        return 0;
    }
    themeEdit.model: Utils.Colors.themeNames
    themeEdit.onCurrentIndexChanged: {
        var selectedTheme = themeEdit.model[themeEdit.currentIndex];
        if (selectedTheme) {
            var theme = Utils.Colors.themeNamesToIdMap[selectedTheme];
            if (theme) {

                Utils.Colors.theme = theme;
            }
        }
    }
}
