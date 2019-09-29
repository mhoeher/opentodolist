import QtQuick 2.4
import QtQuick.Controls 2.12

import "../Utils" as Utils

SettingsPageForm {
    property StackView stack: null

    width: stack.width
    height: stack.height

    onClose: {
        console.warn("Foo");
        stack.pop();
    }
    Component.onCompleted: {
        var theme = Utils.Colors.theme;
        var themes = Utils.Colors.themes;
        for (var i = 0; i < themes.length; ++i) {
            if (theme === themes[i].toLowerCase()) {
                themeEdit.currentIndex = i;
                break;
            }
        }
    }
    themeEdit.model: Utils.Colors.themes
    themeEdit.onCurrentIndexChanged: {
        var idx = themeEdit.currentIndex;
        Utils.Colors.theme = Utils.Colors.themes[idx].toLowerCase();
    }
}
