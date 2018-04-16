#!/bin/env python3

"""
Convert FontAwesome's `icons.json` to QML.

This script creates a QML component which defines
constants for all FontAwesome icons listed in the file
`icons.json` in the FontAwesome package.
"""

import sys


def _dash2cap(name):
    try:
        while True:
            idx = name.index("-")
            pre = name[0:idx]
            suf = name[idx + 2:]
            cap = name[idx + 1:idx + 2].capitalize()
            name = pre + cap + suf
    except ValueError:
        pass
    return name


def _main(argv):
    import json
    if len(argv) != 3:
        print("Usage:")
        msg = "    {} path/to/icons.json path/to/Icons.qml"
        msg = msg.format(argv[0])
        print(msg)
        sys.exit(1)
    with open(argv[1], "r") as file:
        icons = json.load(file)
    lines = []
    lines.append("pragma Singleton")
    lines.append("import QtQuick 2.0")
    lines.append("")
    lines.append("QtObject {")
    prop = '    readonly property string {}: "{}"'
    for key in icons:
        name = "fa-" + key
        name = _dash2cap(name)
        code = "\\u" + icons[key]["unicode"]
        line = prop.format(name, code)
        lines.append(line)
    lines.append("}")
    with open(argv[2], "w") as file:
        file.write("\n".join(lines))


if __name__ == '__main__':
    _main(sys.argv)
