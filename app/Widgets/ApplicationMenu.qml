import QtQuick 2.9
import Qt.labs.platform as P

import OpenTodoList 1.0 as OTL
import "../Controls" as C
import "../Components" as Cmp
import "../Fonts" as Fonts

P.MenuBar {
    id: menuBar

    property Cmp.ApplicationShortcuts shortcuts: null

    component ActionMenuItem: P.MenuItem {
        property C.Action action

        property string type: "item"

        text: action?.text ?? ""
        shortcut: action?.shortcut
        enabled: action?.enabled ?? true
        role: P.MenuItem.NoRole
        onTriggered: action?.triggered()
    }

    component SymbolMenu: P.Menu {
        property string symbol: ""
    }

    component Separator: P.MenuSeparator {
        property string type: "separator"
    }

    SymbolMenu {
        id: fileMenu

        title: qsTr("File")
        symbol: Fonts.Icons.mdiAccountCircle

        ActionMenuItem {
            action: menuBar.shortcuts.newLibrary
        }

        ActionMenuItem {
            action: menuBar.shortcuts.accounts
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.sync
        }

        ActionMenuItem {
            action: menuBar.shortcuts.syncLog
        }

        ActionMenuItem {
            action: menuBar.shortcuts.openLibraryFolder
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.settings
            role: P.MenuItem.PreferencesRole
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.quit
            role: P.MenuItem.QuitRole
        }
    }

    SymbolMenu {
        id: editMenu

        title: qsTr("Edit")
        symbol: Fonts.Icons.mdiEdit

        ActionMenuItem {
            action: menuBar.shortcuts.find
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.addTag
        }

        ActionMenuItem {
            action: menuBar.shortcuts.attachFile
        }

        ActionMenuItem {
            action: menuBar.shortcuts.dueDate
        }

        ActionMenuItem {
            action: menuBar.shortcuts.setColor
        }

        ActionMenuItem {
            action: menuBar.shortcuts.setProgress
        }

        ActionMenuItem {
            action: menuBar.shortcuts.rename
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.copy
        }

        ActionMenuItem {
            action: menuBar.shortcuts.move
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.deleteItem
        }

        ActionMenuItem {
            action: menuBar.shortcuts.deleteCompletedItems
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.markAllItemsAsDone
        }

        ActionMenuItem {
            action: menuBar.shortcuts.markAllItemsAsUndone
        }
    }

    SymbolMenu {
        id: navigateMenu

        title: qsTr("Navigate")
        symbol: Fonts.Icons.mdiViewColumn

        ActionMenuItem {
            action: menuBar.shortcuts.goBack
        }

        ActionMenuItem {
            action: menuBar.shortcuts.leftSidebar
        }

        ActionMenuItem {
            action: menuBar.shortcuts.scrollToTop
        }

        ActionMenuItem {
            action: menuBar.shortcuts.scrollToBottom
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.open
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.sort
        }
    }

    SymbolMenu {
        id: helpMenu

        title: qsTr("Help")
        symbol: Fonts.Icons.mdiMoreVert

        ActionMenuItem {
            action: menuBar.shortcuts.translateTheApp
        }

        Separator {}

        ActionMenuItem {
            action: menuBar.shortcuts.aboutApp
            role: P.MenuItem.AboutRole
        }

        ActionMenuItem {
            action: menuBar.shortcuts.aboutQt
            role: P.MenuItem.AboutQtRole
        }
    }
}
