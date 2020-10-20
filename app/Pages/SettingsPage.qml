import QtQuick 2.4
import QtQuick.Controls 2.12
import "../Components"
import QtQuick.Layouts 1.3


import "../Utils" as Utils


Page {
    id: page

    property alias themeEdit: themeEdit

    signal close

    property alias fontSizeEdit: fontSizeEdit
    property alias languageEdit: languageEdit
    property StackView stack: null

    padding: 10
    footer: DialogButtonBox {
        id: buttons

        standardButtons: DialogButtonBox.Close
    }
    width: stack.width
    height: stack.height

    onClose: {
        stack.pop();
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent

        GridLayout {
            width: scrollView.width
            columns: 2

            Heading {
                text: qsTr("Settings")
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            Heading {
                text: qsTr("User Interface")
                level: 2
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            Label {
                id: label
                text: qsTr("Language:")
            }

            ComboBox {
                id: languageEdit
                Layout.fillWidth: true
                model: Utils.Translations.languages
                textRole: "name"
                currentIndex: {
                    var model = Utils.Translations.languages;
                    for (var i = 0; i < model.count; ++i) {
                        var lang = model.get(i);
                        if (lang.key === translations.language) {
                            return i;
                        }
                    }
                }
                onCurrentIndexChanged: {
                    var lang = Utils.Translations.languages.get(languageEdit.currentIndex);
                    translations.language = lang.key;
                }
            }

            Label {
                text: qsTr("Theme:")
            }

            ComboBox {
                id: themeEdit
                Layout.fillWidth: true
                currentIndex: {
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
                model: Utils.Colors.themeNames
                onCurrentIndexChanged: {
                    var selectedTheme = themeEdit.model[themeEdit.currentIndex];
                    if (selectedTheme) {
                        var theme = Utils.Colors.themeNamesToIdMap[selectedTheme];
                        if (theme) {

                            Utils.Colors.theme = theme;
                        }
                    }
                }
            }

            Label {
                text: qsTr("Font Size:")
            }

            Slider {
                id: fontSizeEdit
                from: 6
                to: 30
                Layout.fillWidth: true
                value: ApplicationWindow.window.font.pixelSize
                live: false
                onValueChanged: ApplicationWindow.window.font.pixelSize = fontSizeEdit.value
            }
        }
    }

    Connections {
        target: buttons
        function onRejected() {
            page.close();
        }
    }
}

