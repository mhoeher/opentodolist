#!/bin/env python3

"""
Convert a Material Design Icons Codepoints file to QML.

This script creates a QML component which defines
constants for all icons listed in the file
`MaterialIcons-Regular.codepoints` in the FontAwesome package.
"""

import fire
from typing import Optional, NamedTuple, List
from pathlib import Path
from textwrap import dedent


class Icon(NamedTuple):
    name: str
    code: str


class CodepointsConverter:
    def __init__(self):
        super().__init__()
        self._input_file: Optional[str] = None
        self._output_file: Optional[str] = None
        self._icons: List[Icon] = []

    def convert(
        self, input_file: Optional[str] = None, output_file: Optional[str] = None
    ):
        """
        Convert Code Points file to QML.

        This commands converts the Material Design Icons codepoints file
        to a QML file, which provides a nice way to access these icons
        in QML.
        """
        if input_file is None:
            input_file = str(
                Path(__file__).parent.parent
                / "app"
                / "Fonts"
                / "material-design-icons"
                / "font"
                / "MaterialIconsOutlined-Regular.codepoints"
            )
        self._input_file = input_file
        self._output_file = output_file
        self._parse()
        self._write()

    def _parse(self):
        icons = self._icons
        with open(self._input_file) as file:
            for line in file:
                chunks = line.split()
                if len(chunks) < 2:
                    continue
                name = chunks[0]
                code_point = chunks[1]
                icons.append(Icon(self._fix_name(name), f"\\u{code_point}"))

    def _fix_name(self, name):
        name = "mdi_" + name
        try:
            while True:
                idx = name.index("_")
                pre = name[0:idx]
                suf = name[idx + 2 :]
                cap = name[idx + 1 : idx + 2].capitalize()
                name = pre + cap + suf
        except ValueError:
            pass
        return name

    def _write(self):
        icons = self._icons
        lines = "\n".join(
            [
                f'    readonly property string {icon.name}: "{icon.code}"'
                for icon in icons
            ]
        )
        content = dedent("""\
        pragma Singleton
        import QtQuick 2.0

        QtObject {{
        {lines}
        }}
        """).format(lines=lines)

        output_file = self._output_file
        if output_file is not None:
            with open(output_file, "w") as file:
                file.write(content)
        else:
            print(content)


if __name__ == "__main__":
    fire.Fire(CodepointsConverter())

# import sys


# def _dash2cap(name):
#     try:
#         while True:
#             idx = name.index("-")
#             pre = name[0:idx]
#             suf = name[idx + 2:]
#             cap = name[idx + 1:idx + 2].capitalize()
#             name = pre + cap + suf
#     except ValueError:
#         pass
#     return name


# def _main(argv):
#     import json
#     if len(argv) != 3:
#         print("Usage:")
#         msg = "    {} path/to/icons.json path/to/Icons.qml"
#         msg = msg.format(argv[0])
#         print(msg)
#         sys.exit(1)
#     with open(argv[1], "r") as file:
#         icons = json.load(file)
#     lines = []
#     lines.append("pragma Singleton")
#     lines.append("import QtQuick 2.0")
#     lines.append("")
#     lines.append("QtObject {")
#     prop = '    readonly property string {}: "{}"'
#     for key in icons:
#         name = "fa-" + key
#         name = _dash2cap(name)
#         code = "\\u" + icons[key]["unicode"]
#         line = prop.format(name, code)
#         lines.append(line)
#     lines.append("}")
#     with open(argv[2], "w") as file:
#         file.write("\n".join(lines))


# if __name__ == '__main__':
#     _main(sys.argv)
