import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0

Item {
    
    property StackView stack: null
    
    function cancel() {}
    
    ColumnLayout {
        anchors {
            fill: parent
            margins: Globals.defaultMargin
        }
        
        Text {
            text: qsTr("OpenTodoList")
            font {
                pixelSize: Globals.fontPixelSize * 2
                bold: true
            }
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Layout.fillWidth: true
        }
        
        Text {
            text: qsTr("A todo and task managing application.")
            font.bold: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Layout.fillWidth: true
        }
        
        Text {
            text: qsTr("(c) RPdev 2013-2016, version %1").arg(applicationVersion)
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Layout.fillWidth: true
        }
        
        Text {
            text: "<a href='%1'>%1</a>".arg("https://www.rpdev.net/wordpress/apps/opentodolist/")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Layout.fillWidth: true
            onLinkActivated: Qt.openUrlExternally(link)
        }
        
        Text {
            text: qsTr("OpenTodoList is released under the terms of the GNU General Public " +
                       "License version 3 or (at your choice) any later version. You can find a " +
                       "copy of the license below. Additionally, several libraries and resources " +
                       "are used. For detailed information about their license terms, please " +
                       "refer to the <a href='3rdparty'>3rd Party Software</a> page.")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            Layout.fillWidth: true
            onLinkActivated: {
                if (link == "3rdparty") {
                    stack.push({item: thirdPartyPage})
                }
            }
        }
        
        TextArea {
            readOnly: true
            text: App.readFile(":/res/COPYING")
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        
        Component {
            id: thirdPartyPage
            
            AboutThirdPartyPage {}
        }
    }
}
