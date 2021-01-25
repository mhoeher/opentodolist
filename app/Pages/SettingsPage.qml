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

    padding: 0
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
        padding: Utils.AppSettings.mediumSpace

        GridLayout {
            implicitWidth: childrenRect.width
            implicitHeight: childrenRect.height
            width: scrollView.availableWidth
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
                to: 20
                Layout.fillWidth: true
                value: Utils.AppSettings.customFontSize
                live: false
                enabled: Utils.AppSettings.useCustomFontSize
                onValueChanged: Utils.AppSettings.customFontSize = fontSizeEdit.value
            }

            Empty {}

            CheckBox {
                text: qsTr("Use custom font size")
                checked: Utils.AppSettings.useCustomFontSize
                onCheckedChanged: Utils.AppSettings.useCustomFontSize = checked
                Layout.fillWidth: true
            }

            Empty {}

            CheckBox {
                text: qsTr("Use Compact Style")
                checked: Utils.AppSettings.useDenseVariant
                onCheckedChanged: Utils.AppSettings.useDenseVariant = checked
                Layout.fillWidth: true
            }

            Empty {}

            Label {
                text: qsTr("Reduce space between components and reduce the font size.\n\n" +
                           "<em>Requires a restart of the app.</em>")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                Layout.fillWidth: true
            }

            Empty {}

            CheckBox {
                text: qsTr("Use compact todo lists")
                checked: Utils.AppSettings.useCompactTodoLists
                onCheckedChanged: Utils.AppSettings.useCompactTodoLists = checked
                Layout.fillWidth: true
            }

            Empty {}

            Label {
                Layout.fillWidth: true
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: qsTr("Reduce the padding in todo and task listings to fit more items on " +
                           "the screen.")
            }

            Empty {}

            CheckBox {
                text: qsTr("Override Scaling Factor")
                checked: Utils.AppSettings.overrideUiScaling
                onCheckedChanged: Utils.AppSettings.overrideUiScaling = checked
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Scale Factor:")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            SpinBox {
                from: 50
                to: 300
                value: Utils.AppSettings.uiScaling
                stepSize: 25
                editable: false
                textFromValue: function(value, locale) { return Number(value).toLocaleString(locale, 'f', 0) + "%"; }
                onValueChanged: Utils.AppSettings.uiScaling = value
                Layout.fillWidth: true
                enabled: Utils.AppSettings.overrideUiScaling
            }

            Empty {}

            Label {
                text: qsTr("Use this to manually scale the user interface. By default, the app " +
                           "should adapt automatically according to your device configuration. " +
                           "If this does not work properly, you can set a custom scaling factor " +
                           "here.\n\n" +
                           "This requires a restart of the app.")
                Layout.fillWidth: true
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

