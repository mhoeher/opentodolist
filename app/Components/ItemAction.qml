import QtQuick 2.0

import "../Controls" as C
import OpenTodoList 1.0 as OTL

C.Action {
    property OTL.LibraryItem item: null
    property string symbol: ""
    property bool hideButton: false
}
