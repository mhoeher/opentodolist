import QtQuick 2.0
import OpenTodoList 1.0 as OTL
import "../Components" as Components
import "../Fonts" as Fonts
import "../Utils" as Utils

Components.ItemAction {
    property OTL.Library library: null
    property OTL.TodoList todoList: null

    symbol: Fonts.Icons.mdiUpgrade
    text: qsTr("Promote")
    onTriggered: Utils.ItemUtils.promoteTask(item, todoList, library)
}
