import QtQuick 2.0

import OpenTodoList 1.0 as OTL

Item {
    id: root

    property alias accounts: listModel

    ListModel {
        id: listModel
        dynamicRoles: true
    }


    QtObject {
        id: d

        function refresh() {
            listModel.clear();
            var uids = OTL.Application.accountUids();
            for (var i = 0; i < uids.length; ++i) {
                var account = OTL.Application.loadAccount(uids[i]);
                if (account !== null) {
                    listModel.append({
                                    "name": account.name,
                                    "type": account.type,
                                    "username": account.username,
                                    "password": account.password,
                                    "account": account
                                });
                }
            }
        }
    }

    Component.onCompleted: d.refresh()

    Connections {
        target: OTL.Application
        onAccountsChanged: d.refresh()
    }
}
