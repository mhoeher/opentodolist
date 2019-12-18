pragma Singleton

import QtQuick 2.0

Item {

    readonly property ListModel languages: languageModel

    ListModel {
        id: languageModel

        ListElement {
            key: ""
            name: qsTr("System Language")
        }
        ListElement {
            key: "de"
            name: qsTr("German")
        }
        ListElement {
            key: "en"
            name: qsTr("English")
        }
        ListElement {
            key: "fr"
            name: qsTr("French")
        }
    }

    Component.onCompleted: {
        var availableLangs = translations.allLanguages();
        for (var i = 0; i < availableLangs.length; ++i) {
            var availableLang = availableLangs[i];
            var found = false;
            for (var j = 0; j < languageModel.count; ++j) {
                var lang = languageModel.get(j);
                if (lang.key === availableLang) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                languageModel.append(
                    {
                        "key": availableLang,
                        "name": availableLang
                    });
            }
        }
    }

}
