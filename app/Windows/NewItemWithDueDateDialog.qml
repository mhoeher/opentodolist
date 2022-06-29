import QtQuick 2.0
import QtQml.Models 2.1
import QtQuick.Layouts 1.0

import Qt.labs.settings 1.0

import OpenTodoList 1.0 as OTL

import "../Components"
import "../Controls" as C
import "../Fonts"
import "../Utils"
import "."

CenteredDialog {
    id: dialog

    width: Math.min(1000, parent.width * 0.9)

    property OTL.Library library: null
    readonly property alias parentItem: parentEdit.currentValue
    readonly property alias itemType: d.itemType
    readonly property alias newItemTitle: titleEdit.displayText
    readonly property alias selectedLibrary: d.selectedLibrary

    function newItemDueOn() {
        // Note: We make this a function to force evaluation when new items are
        // created without changing the due date combo box value. Otherwise, we "cache"
        // the dates when the view remains open for a long time.
        switch (dueDateEdit.displayText) {
        case d.today:
            return DateUtils.today()
        case d.tomorrow:
            return DateUtils.tomorrow()
        case d.thisWeek:
            return DateUtils.endOfThisWeek()
        case d.nextWeek:
            return DateUtils.endOfNextWeek()
        default:
            return d.customDate
        }
    }

    function createNote() {
        d.itemType = "Note"
        d.open()
    }

    function createTodoList() {
        d.itemType = "TodoList"
        d.open()
    }

    function createTodo() {
        d.itemType = "Todo"
        d.open()
    }

    Settings {
        id: settings

        category: "NewItemWithDueDateDialog/" + d.itemType
    }

    Settings {
        id: pageSettings

        category: "NewItemWithDueDateDialog"

        property var lastLibraryUid: null
    }

    standardButtons: C.DialogButtonBox.Ok | C.DialogButtonBox.Cancel
    Component.onCompleted: {
        let okButton = standardButton(C.DialogButtonBox.Ok)
        d.okButton = okButton
        okButton.enabled = Qt.binding(function () {
            let result = dialog.newItemTitle !== "" && DateUtils.validDate(
                    dialog.newItemDueOn())
            if (d.itemType === "Todo") {
                result = result && !!parentEdit.currentValue
            }
            if (!d.selectedLibrary) {
                result = false
            }
            return result
        })
    }

    QtObject {
        id: d

        function open() {
            titleEdit.clear()
            customDate = null
            dueDateEdit.currentIndex = 0
            dialog.open()
            titleEdit.forceActiveFocus()
        }

        property string itemType: ""
        property var customDate: null
        property var okButton: null
        readonly property bool needParent: itemType === "Todo"
        readonly property string today: qsTr("Today")
        readonly property string tomorrow: qsTr("Tomorrow")
        readonly property string thisWeek: qsTr("This Week")
        readonly property string nextWeek: qsTr("Next Week")
        readonly property string selectDate: qsTr("Select...")
        property OTL.Library selectedLibrary: {
            if (dialog.library) {
                return dialog.library
            } else if (libraryEdit.currentValue) {
                return libraryEdit.currentValue
            } else {
                return null
            }
        }

        function accept() {
            if (okButton.enabled) {
                dialog.accept()
            }
        }
    }

    ListModel {
        id: dueDates
        dynamicRoles: true
    }

    GridLayout {
        columns: 2
        width: dialog.availableWidth

        C.Label {
            text: qsTr("Title:")
        }

        C.TextField {
            id: titleEdit
            placeholderText: qsTr("The title for your new item...")
            Layout.fillWidth: true
            onAccepted: d.accept()
        }

        C.Label {
            text: qsTr("Library")
            visible: !dialog.library
        }

        C.ComboBox {
            id: libraryEdit
            visible: !dialog.library
            Layout.fillWidth: true
            model: OTL.LibrariesModel {
                cache: dialog.library ? null : OTL.Application.cache
            }
            textRole: "name"
            valueRole: "library"
            delegate: C.ItemDelegate {
                width: parent.width
                text: name
                onClicked: {
                    pageSettings.lastLibraryUid = library.uid
                }
            }
            onCountChanged: {
                if (!currentValue) {
                    let lastUid = pageSettings.lastLibraryUid
                    if (lastUid !== null) {
                        for (var i = 0; i < libraryEdit.count; ++i) {
                            if (libraryEdit.valueAt(i).uid === lastUid) {
                                libraryEdit.currentIndex = i
                                break
                            }
                        }
                    }
                }
            }
        }

        C.Label {
            text: qsTr("Create in:")
            visible: d.needParent
        }

        C.ComboBox {
            id: parentEdit

            visible: d.needParent
            Layout.fillWidth: true
            model: OTL.ItemsModel {
                cache: d.needParent
                       && d.selectedLibrary ? OTL.Application.cache : null
                parentItem: d.selectedLibrary ? d.selectedLibrary.uid : ""
                itemType: "TodoList"
            }
            onCountChanged: {
                if (!parentEdit.currentValue && d.selectedLibrary) {
                    let lastUid = settings.value(d.selectedLibrary.uid, null)
                    if (lastUid !== null) {
                        for (var i = 0; i < parentEdit.count; ++i) {
                            if (parentEdit.valueAt(i).uid === lastUid) {
                                parentEdit.currentIndex = i
                                break
                            }
                        }
                    }
                }
            }

            textRole: "title"
            valueRole: "object"
            delegate: C.ItemDelegate {
                width: parent.width
                text: title
                onClicked: {
                    settings.setValue(d.selectedLibrary.uid, uid)
                }
            }
        }

        C.Label {
            text: qsTr("Due on:")
        }

        C.ComboBox {
            id: dueDateEdit

            model: [d.today, d.tomorrow, d.thisWeek, d.nextWeek, d.selectDate]
            displayText: {
                if (DateUtils.validDate(d.customDate)) {
                    return DateUtils.format(d.customDate)
                } else {
                    return currentText
                }
            }
            delegate: C.ItemDelegate {
                text: modelData
                width: parent.width
                onClicked: {
                    if (modelData === d.selectDate) {
                        dateSelectionDialog.open()
                    } else {
                        d.customDate = null
                    }
                }
            }
            Layout.fillWidth: true

            DateSelectionDialog {
                id: dateSelectionDialog
                onAccepted: {
                    d.customDate = selectedDate
                    if (!DateUtils.validDate(d.customDate)) {
                        dueDateEdit.currentIndex = 0
                    }
                }
                onRejected: {
                    if (!DateUtils.validDate(d.customDate)) {
                        dueDateEdit.currentIndex = 0
                    }
                }
            }
        }
    }
}
