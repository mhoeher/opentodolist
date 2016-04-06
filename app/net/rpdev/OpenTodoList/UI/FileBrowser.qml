import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4

FileDialog {
    id: dialog

    property StackView stack: null
    property var fileNameExtensions
    
    nameFilters: {
        var result = null;
        if (fileNameExtensions) {
            result = qsTr("Supported Files") + " (";
            for (var i = 0; i < fileNameExtensions.length; ++i) {
                result += "*." + fileNameExtensions[i] + " ";
            }
            result += ")";
         }
        return result;
    }
}
