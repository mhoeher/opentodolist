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
    width: parent.width
    height: parent.height

    onClose: {
        stack.pop()
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
                popup: C.Popup {
                    y: page.availableHeight / 20
                    width: languageEdit.width
                    implicitHeight: contentItem.implicitHeight

                    contentItem: ListView {
                        clip: true
                        implicitHeight: Math.min(contentHeight,
                                                 page.availableHeight / 2)
                        model: languageEdit.popup.visible ? languageEdit.delegateModel : null
                        currentIndex: languageEdit.highlightedIndex

                        C.ScrollIndicator.vertical: C.ScrollIndicator {}
                    }
                }
                currentIndex: {
                    var model = translations.availableLanguages()
                    for (var i = 0; i < model.length; ++i) {
                        var lang = model[i]
                        if (lang.code === translations.language) {
                            return i
                        }
                    }
                    // The first entry is always the system language - hence, if we didn't match
                    // anything else, choose that:
                    return 0
                }
                onCurrentValueChanged: {
                    if (enabled) {
                        // WA for rpdev/opentodolist#540: We get a double change, the first
                        // one is the user originated one, the second a "bogus" one which will always
                        // reset to system language. Disable the edit and only accept changes after a
                        // "cooldown".
                        enabled = false
                        reenableLanguageChangeTimer.start()

                        // WA for rpdev/opentodolist#512:
                        // Remeber current theme by manually saving it before
                        // changing the language:
                        let theme = Utils.Colors.theme
                        translations.language = currentValue
                        Utils.Colors.theme = theme
                    }
                }

                Timer {
                    id: reenableLanguageChangeTimer
                    interval: 1000
                    repeat: false
                    onTriggered: languageEdit.enabled = true
                }
            }

            C.Label {
                text: qsTr("Theme:")
            }

            C.ComboBox {
                id: themeEdit
                Layout.fillWidth: true

                model: Utils.Colors.themes
                valueRole: "name"
                textRole: "title"
                onActivated: {
                    Utils.Colors.theme = currentValue
                }
                Component.onCompleted: {
                    for (var i = 0; i < Utils.Colors.themes.length; ++i) {
                        if (Utils.Colors.themes[i].name === Utils.Colors.theme) {
                            return i
                        }
                    }
                    return 0
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
                text: qsTr("Reduce space between components and reduce the font size.\n\n"
                           + "<em>Requires a restart of the app.</em>")
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
                text: qsTr("Reduce the padding in todo and task listings to fit more items on "
                           + "the screen.")
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
                textFromValue: function (value, locale) {
                    return Number(value).toLocaleString(locale, 'f', 0) + "%"
                }
                onValueChanged: Utils.AppSettings.uiScaling = value
                Layout.fillWidth: true
                enabled: Utils.AppSettings.overrideUiScaling
            }

            Empty {}

            C.Label {
                text: qsTr(
                          "Use this to manually scale the user interface. By default, the app "
                          + "should adapt automatically according to your device configuration. "
                          + "If this does not work properly, you can set a custom scaling factor "
                          + "here.\n\n" + "This requires a restart of the app.")
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

            Empty {}

            C.CheckBox {
                text: qsTr("Show notes excerpt in listings")
                checked: Utils.AppSettings.showNotesExcepts
                onCheckedChanged: Utils.AppSettings.showNotesExcepts = checked
            }
        }
    }

    Connections {
        target: buttons
        function onRejected() {
            page.close()
        }
    }
}
