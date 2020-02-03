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

        property var accountObjects: ([])

        function refresh() {
            var accountList = [];
            listModel.clear();
            var uids = OTL.Application.accountUids();
            for (var i = 0; i < uids.length; ++i) {
                var account = OTL.Application.loadAccount(uids[i]);
                if (account !== null) {
                    accountList.push(account);
                    listModel.append({
                                    "name": account.name,
                                    "type": account.type,
                                    "username": account.username,
                                    "password": account.password,
                                    "account": account
                                });
                }
            }
            d.accountObjects = accountList;
        }
    }

    Component.onCompleted: d.refresh()

    Connections {
        target: OTL.Application
        onAccountsChanged: d.refresh()
    }
}
