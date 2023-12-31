import QtQuick 2.0

import "../Controls" as C
import "../Windows" as W

Item {
    property C.StackView stackView: null

    function openLibrary(url, library) {
        let properties = {
            "library": library
        }
        stackView.push(Qt.resolvedUrl("../Pages/LibraryPage.qml"), properties)
    }

    function openNote(url, note, library) {
        let properties = {
            "library": library,
            "item": note
        }
        stackView.push(Qt.resolvedUrl("../Pages/NotePage.qml"), properties)
    }

    function openTodoList(url, todoList, library) {
        let properties = {
            "library": library,
            "item": todoList
        }
        stackView.push(Qt.resolvedUrl("../Pages/TodoListPage.qml"), properties)
    }

    function openTodo(url, todo, todoList, library) {
        let properties = {
            "library": library,
            "item": todo,
            "todoList": todoList
        }
        stackView.push(Qt.resolvedUrl("../Pages/TodoPage.qml"), properties)
    }

    function openImage(url, image, library) {
        let properties = {
            "library": library,
            "item": image
        }
        stackView.push(Qt.resolvedUrl("../Pages/ImagePage.qml"), properties)
    }

    function warnLinkTargetNotFound() {
        notFoundDialog.open()
    }

    W.CenteredDialog {
        id: notFoundDialog

        title: qsTr("Uuups... seems that's a dead end...")
        width: idealDialogWidth
        standardButtons: C.Dialog.Close

        C.Label {
            width: notFoundDialog.availableWidth
            text: qsTr("Sorry, that link does not lead to any page or item that is present on this device. Check if the library or item to which the link points is synchronized on this device and try again.")
        }
    }
}
