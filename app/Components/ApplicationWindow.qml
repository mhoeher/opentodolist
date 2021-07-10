import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2
import QtQuick.Controls.Material 2.12

import "../Utils"

QQC2.ApplicationWindow {
    id: appWindow

    Material.theme: {
        switch (Colors.theme) {
        case Colors.lightTheme: return Material.Light;
        case Colors.darkTheme: return Material.Dark;
        case Colors.systemTheme: return Material.System;
        }
    }

    Component.onCompleted: {
        ItemUtils.window = appWindow
    }
}
