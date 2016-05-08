pragma Singleton

import QtQuick 2.0
import QtQuick.Controls 1.4

import net.rpdev.OpenTodoList 1.0
import net.rpdev.OpenTodoList.UI 1.0


Item {
    id: globals
    
    property QtObject appWindow: null
    property int fontPixelSize: typeof(defaultFontPixelSize) === "number" ? defaultFontPixelSize : 32
    property int minButtonHeight: fontPixelSize * (touchEnabled ? 2.5 : 2)
    property bool touchEnabled: enableTouchOptimizations
    property int defaultMargin: fontPixelSize
    
    readonly property int defaultAnimationTime: 500
    
    function file(relativeName) {
        var result = qmlBaseDirectory + "/" + relativeName;
        result.replace("//", "/");
        return result;
    }
    
}
