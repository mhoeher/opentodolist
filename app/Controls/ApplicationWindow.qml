import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2
import QtQuick.Controls.Material 2.12

import "../Utils" as Utils

QQC2.ApplicationWindow {
    id: appWindow

    Material.theme: {
        switch (Utils.Colors.theme) {
        case Utils.Colors.lightTheme:
            return Material.Light
        case Utils.Colors.darkTheme:
            return Material.Dark
        case Utils.Colors.systemTheme:
            return Material.System
        }
    }

    property Utils.ItemUtils itemUtils: Utils.ItemUtils {
        window: appWindow
    }
}
