import QtQuick 2.4
import QtQuick.Layouts 1.3

import "../Components"
import "../Controls" as C
import "../Utils" as Utils


C.Page {
    id: page

    property alias themeEdit: themeEdit

    signal close

    property alias fontSizeEdit: fontSizeEdit
    property alias languageEdit: languageEdit
    property C.StackView stack: null

    padding: 0
    footer: C.DialogButtonBox {
        id: buttons

        standardButtons: C.DialogButtonBox.Close
    }
    width: stack.width
    height: stack.height

    onClose: {
        stack.pop();
    }

    C.ScrollView {
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

            C.Label {
                id: label
                text: qsTr("Language:")
            }

            C.ComboBox {
                id: languageEdit
                Layout.fillWidth: true
                model: translations.availableLanguages()
                textRole: "name"
                valueRole: "code"
                currentIndex: {
                    var model = translations.availableLanguages();
                    for (var i = 0; i < model.length; ++i) {
                        var lang = model[i];
                        if (lang.code === translations.language) {
                            return i;
                        }
                    }
                    // The first entry is always the system language - hence, if we didn't match
                    // anything else, choose that:
                    return 0;
                }
                onCurrentValueChanged: translations.language = currentValue
            }

            C.Label {
                text: qsTr("Theme:")
            }

            C.ComboBox {
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

            C.Label {
                text: qsTr("System Tray:")
                visible: Utils.AppSettings.supportsQuickEditor
            }

            C.CheckBox {
                checked: Utils.AppSettings.showQuickNotesEditorOnSystemTrayClick
                text: qsTr("Open Quick Notes Editor on Click")
                onCheckedChanged: Utils.AppSettings.showQuickNotesEditorOnSystemTrayClick = checked
                visible: Utils.AppSettings.supportsQuickEditor
            }

            C.Label {
                text: qsTr("Font Size:")
            }

            C.Slider {
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

            C.CheckBox {
                text: qsTr("Use custom font size")
                checked: Utils.AppSettings.useCustomFontSize
                onCheckedChanged: Utils.AppSettings.useCustomFontSize = checked
                Layout.fillWidth: true
            }

            Empty {}

            C.CheckBox {
                text: qsTr("Desktop Mode")
                checked: Utils.AppSettings.desktopMode
                onCheckedChanged: Utils.AppSettings.desktopMode = checked
                Layout.fillWidth: true
            }

            Empty {}

            C.CheckBox {
                text: qsTr("Use Compact Style")
                checked: Utils.AppSettings.useDenseVariant
                onCheckedChanged: Utils.AppSettings.useDenseVariant = checked
                Layout.fillWidth: true
            }

            Empty {}

            C.Label {
                text: qsTr("Reduce space between components and reduce the font size.\n\n" +
                           "<em>Requires a restart of the app.</em>")
                Layout.fillWidth: true
            }

            Empty {}

            C.CheckBox {
                text: qsTr("Use compact todo lists")
                checked: Utils.AppSettings.useCompactTodoLists
                onCheckedChanged: Utils.AppSettings.useCompactTodoLists = checked
                Layout.fillWidth: true
            }

            Empty {}

            C.Label {
                Layout.fillWidth: true
                text: qsTr("Reduce the padding in todo and task listings to fit more items on " +
                           "the screen.")
            }

            Empty {}

            C.CheckBox {
                text: qsTr("Override Scaling Factor")
                checked: Utils.AppSettings.overrideUiScaling
                onCheckedChanged: Utils.AppSettings.overrideUiScaling = checked
                Layout.fillWidth: true
            }

            C.Label {
                text: qsTr("Scale Factor:")
            }

            C.SpinBox {
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

            C.Label {
                text: qsTr("Use this to manually scale the user interface. By default, the app " +
                           "should adapt automatically according to your device configuration. " +
                           "If this does not work properly, you can set a custom scaling factor " +
                           "here.\n\n" +
                           "This requires a restart of the app.")
                Layout.fillWidth: true
            }

            C.Label {
                text: qsTr("Library Item Size:")
            }

            C.SpinBox {
                from: 5
                to: 500
                value: Utils.AppSettings.libraryItemWidthScaleFactor
                stepSize: 5
                editable: false
                onValueChanged: Utils.AppSettings.libraryItemWidthScaleFactor = value
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

