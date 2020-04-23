import QtQuick 2.0
import QtQuick.Controls 2.5
import "./Windows"
import QtQuick.Window 2.0

MainWindow {
    Component.onCompleted: {
        // Check if the maximized option is set (from a previous run) and depending on this
        // bring up the Window in maximized mode
        if (visibility === Window.Maximized) {
            showMaximized();
        } else {
            show();
        }
    }
}
