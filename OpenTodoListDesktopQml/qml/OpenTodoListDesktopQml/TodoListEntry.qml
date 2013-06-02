import QtQuick 2.0

Rectangle {
    id: entry
    color: activePalette.button
    width: parent.width - 10
    x: parent.x + 5
    height: childrenRect.height
    
    property QtObject todo: null
    property alias containsMouse: mouseArea.containsMouse
    
    signal clicked
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        
        onClicked: entry.clicked()
    }
    
    Row {
        Rectangle {
            id: checkMark
            border.width: 1
            border.color: 'black'
            color: 'white'
            width: 20
            height: 20
            
            Rectangle {
                id: checkMarkInner
                width: 10
                height: 10
                x: 5
                y: 5
                color: Qt.darker( activePalette.button )
                opacity: 0
            }
            MouseArea {
                anchors.fill: parent
                onClicked: { 
                    entry.todo.progress = ( entry.todo.progress == 100 ? 0 : 100 ); 
                }
            }
            
            states: [ 
                State {
                    name: "checked"
                    when: entry.todo.progress == 100
                    PropertyChanges {
                        target: checkMarkInner
                        opacity: 1
                    }
                }
            ]
            
            transitions: [
                Transition {
                    from: ""
                    to: "checked"
                    reversible: true
                    NumberAnimation {
                        duration: 300
                        easing.type: Easing.Linear
                        properties: "opacity"
                    }
                }
            ]
        }
        
        Text {
            id: label
            text: entry.todo.title
        }
    }
    
    states: [
        State {
            name: "hovered"
            when: containsMouse
            PropertyChanges {
                target: entry
                color: Qt.lighter( activePalette.button )
            }
        }
    ]
    
    transitions: [
        Transition {
            from: ""
            to: "hovered"
            reversible: true
            ColorAnimation {
                duration: 200
            }
        }
    ]
    
}