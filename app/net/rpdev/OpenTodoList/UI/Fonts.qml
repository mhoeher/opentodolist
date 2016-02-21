pragma Singleton

import QtQuick 2.5

QtObject {
    id: fonts

    property FontLoader symbols: FontLoader {
        id: symbolsFont
        
        property alias family: symbolsFont.name
        
        readonly property string faStickyNoteO: "\uf24a"
        readonly property string faPlus: "\uf067"
        
        source: "fontawesome-webfont.ttf"
    }

}
