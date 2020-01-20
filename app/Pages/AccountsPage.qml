import QtQuick 2.4

AccountsPageForm {
    id: page

    signal openPage(var component, var properties)

    roundButton.onClicked: {
        page.openPage(Qt.resolvedUrl("./AccountTypeSelectionPage.qml"), {});
    }
}
