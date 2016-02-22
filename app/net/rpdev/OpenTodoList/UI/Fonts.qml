pragma Singleton

import QtQuick 2.5

QtObject {
    id: fonts

    property FontLoader symbols: FontLoader {
        id: symbolsFont
        
        property alias family: symbolsFont.name
        
        readonly property string faStickyNoteO: "\uf24a"
        readonly property string faPlus: "\uf067"
        readonly property string faAngleLeft: "\uf104"
        readonly property string faFolderOpenO: "\uf115"
        readonly property string faCheckSquareO: "\uf046"
        readonly property string faPictureO: "\uf03e"
        readonly property string faPencilSquareO: "\uf044"
        readonly property string faTrashO: "\uf014"
        readonly property string faCheck: "\uf00c"
        
        source: "fontawesome-webfont.ttf"
    }

}
