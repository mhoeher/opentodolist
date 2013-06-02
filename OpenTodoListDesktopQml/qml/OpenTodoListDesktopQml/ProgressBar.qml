import QtQuick 2.0

Rectangle {
    id: progressBar
    
    property int minValue: 0
    property int maxValue: 100
    property int value: 0
    property real progress: ( value - minValue ) / ( maxValue - minValue )
    
    width: 100
    height: 20
    radius: 4
    clip: true
    
    Rectangle {
        id: inner
        
        property int progress: parent.width * progressBar.progress
        
        anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
        width: progress
        color: 'green'
        
        Behavior on width { SmoothedAnimation { velocity: 1200 } }
    }
    
    Text {
        anchors.centerIn: parent
        text: Math.floor( progressBar.progress * 100 ) + "%"
    }
    
}