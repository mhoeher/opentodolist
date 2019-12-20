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

    languageEdit.model: Utils.Translations.languages
    languageEdit.textRole: "name"
    languageEdit.currentIndex: {
        var model = Utils.Translations.languages;
        for (var i = 0; i < model.count; ++i) {
            var lang = model.get(i);
            if (lang.key === translations.language) {
                return i;
            }
        }
    }
    languageEdit.onCurrentIndexChanged: {
        var lang = Utils.Translations.languages.get(languageEdit.currentIndex);
        translations.language = lang.key;
    }
}
