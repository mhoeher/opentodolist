import QtQuick 2.10
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12

import OpenTodoList 1.0 as OTL

import "../Components" as Components
import "../Windows"
import "../Fonts"
import Utils 1.0 as Utils

NewLibraryPageForm {
    id: page

    signal openPage(var component, var properties)
    signal libraryCreated(OTL.Library library)

    addLocalLibraryDelegate.onClicked: page.openPage(newLocalLibraryPage, {})
    addFolderAsLibraryDelegate.onClicked: page.openPage(newLibraryInFolderPage, {})
    accountsList.model: accountsModel.accounts
    addAccountButton.onClicked: page.openPage(Qt.resolvedUrl("./AccountTypeSelectionPage.qml"), {
                                                  anchorPage: page
                                                  });
    onAccountSelected: page.openPage(newLibaryFromAccountPage, {account: account })


    /* On iOS, we currently have no notion of a "file system" in that sense. So
      don't even show this to the user. */
    addFolderAsLibraryDelegate.visible: Qt.platform.os !== "ios"


    Component {
        id: newLocalLibraryPage

        NewLocalLibraryPage {
            onLibraryCreated: page.libraryCreated(library)
        }
    }

    Component {
        id: newLibraryInFolderPage

        NewLibraryInFolderPage {
            onLibraryCreated: page.libraryCreated(library)
        }
    }

    Component {
        id: newLibaryFromAccountPage

        NewLibraryFromAccountPage {
            onLibraryCreated: page.libraryCreated(library)
        }
    }

    Utils.Accounts {
        id: accountsModel
    }
}
