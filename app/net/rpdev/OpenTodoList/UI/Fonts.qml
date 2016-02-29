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
        readonly property string faSquareO: "\uf096"
        readonly property string faPictureO: "\uf03e"
        readonly property string faPencilSquareO: "\uf044"
        readonly property string faTrashO: "\uf014"
        readonly property string faCheck: "\uf00c"
        
        readonly property string faBold: "\uf032"        
        readonly property string faItalic: "\uf033"
        readonly property string faUnderline: "\uf0cd"
        readonly property string faStrikethrough: "\uf0cc"
        
        readonly property string faList: "\uf03a"
        readonly property string faListOl: "\uf0cb"
        
        readonly property string faAlignLeft: "\uf036"
        readonly property string faAlignRight: "\uf038"
        readonly property string faAlignCenter: "\uf037"
        readonly property string faAlignJustify: "\uf039"
        
        readonly property string faCopy: "\uf0c5"
        readonly property string faCut: "\uf0c4"
        readonly property string faPaste: "\uf0ea"
        
        readonly property string faIndent: "\uf03c"
        readonly property string faOutdent: "\uf03b"
        
        
        source: "fontawesome-webfont.ttf"
    }

}
