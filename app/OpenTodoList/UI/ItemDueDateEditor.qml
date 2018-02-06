import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import OpenTodoList 1.0
import OpenTodoList.UI 1.0


RowLayout {
    id: root

    property ComplexItem item: null

    QtObject {
        id: d

        property bool validDate: root.item.dueTo.getTime() === root.item.dueTo.getTime()
    }

    Symbol {
        symbol: d.validDate ? Fonts.symbols.faCalendar : Fonts.symbols.faCalendarO
    }

    Label {
        Layout.fillWidth: true
        text: d.validDate ? qsTr("Due on") + " " + root.item.dueTo.toLocaleDateString() : ""
        wrapMode: Text.WordWrap
    }

    Button {
        text: qsTr("Set Due Date")
        onClicked: {
            dialog.selectedDate = root.item.dueTo;
            dialog.open();
        }
    }

    DateSelectionDialog {
        id: dialog
        onAccepted: root.item.dueTo = selectedDate
    }
}
