pragma Singleton

import QtQuick 2.0
import OpenTodoList 1.0 as OTL
import "." as Utils

OTL.SyntaxHighlighter {
    theme: Utils.Colors.usingDarkColorTheme ? OTL.SyntaxHighlighter.Dark : OTL.SyntaxHighlighter.Light
}
