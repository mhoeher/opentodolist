import QtQuick 2.10

import "." as Components
import "../Fonts" as Fonts
import "../Utils" as Utils
import "../Controls" as C

Item {
    id: root

    property bool refreshEnabled: true
    property Flickable flickable

    signal refresh()

    clip: true
    width: 200
    height: 200
    implicitWidth: 200
    implicitHeight: 200


    Binding {
        target: pullIndicator
        when: root.refreshEnabled && root.flickable.dragging && root.flickable.verticalOvershoot < 0
        property: "y"
        value: Math.min(-root.flickable.verticalOvershoot, pullIndicator.height * 1.5) -
               pullIndicator.height
    }

    QtObject {
        id: d

        property bool doRefreshAfterDrag: false

        property Flickable flickable: {
            for (let i = 0; i < contentItem.children.length; ++i) {
                let c = contentItem.children[i];
                if (c instanceof Flickable) {
                    return c;
                }
            }
            return null;
        }
    }

    Connections {
        target: root.flickable
        function onDragEnded() {
            if (root.refreshEnabled && d.doRefreshAfterDrag) {
                root.refresh();
            }
        }
    }

    C.RoundButton {
        id: pullIndicator

        y: -height
        width: Utils.AppSettings.effectiveFontMetrics.height * 3
        height: width
        anchors.horizontalCenter: parent.horizontalCenter
        contentItem: C.Label {
            anchors.centerIn: parent
            width: parent.width * 0.8
            height: parent.height * 0.8
            font.family: Fonts.Fonts.icons
            font.pixelSize: height
            text: Fonts.Icons.mdiSync
            opacity: (pullIndicator.y + pullIndicator.height) / (1.5 * pullIndicator.height)
            onOpacityChanged: if (root.flickable.dragging) {
                                  d.doRefreshAfterDrag = opacity >= 0.95;
                              }
        }
        rotation: contentItem.opacity * 360
        transformOrigin: Item.Center
    }
}
