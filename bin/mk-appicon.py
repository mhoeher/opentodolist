#!/usr/bin/env python


def _mkdirs(path):
    from os import makedirs
    try:
        makedirs(path)
    except OSError:
        pass  # Directory already exists


def mk_appicon(icon, icon_transparent, icon_badge, output_dir, inkscape=None):
    from os.path import join
    from os.path import abspath
    from subprocess import check_call
    if inkscape is None:
        inkscape = "inkscape"
    _mkdirs(output_dir)
    icon = abspath(icon)
    output_dir = abspath(output_dir)

    # Create Icons for iOS:
    icons = [
        {"name": "Icon-24@2x.png", "size": 48},
        {"name": "Icon-27.5@2x.png", "size": 55},
        {"name": "Icon-29@2x.png", "size": 58},
        {"name": "Icon-29@3x.png", "size": 87},
        {"name": "Icon-40@2x.png", "size": 80},
        {"name": "Icon-44@2x.png", "size": 88},
        {"name": "Icon-86@2x.png", "size": 172},
        {"name": "Icon-98@2x.png", "size": 196},
        {"name": "Icon-App-20x20@1x.png", "size": 20},
        {"name": "Icon-App-20x20@2x-1.png", "size": 40},
        {"name": "Icon-App-20x20@2x.png", "size": 40},
        {"name": "Icon-App-20x20@3x.png", "size": 60},
        {"name": "Icon-App-29x29@1x.png", "size": 29},
        {"name": "Icon-App-29x29@2x-1.png", "size": 58},
        {"name": "Icon-App-29x29@2x.png", "size": 58},
        {"name": "Icon-App-29x29@3x.png", "size": 87},
        {"name": "Icon-App-40x40@1x.png", "size": 40},
        {"name": "Icon-App-40x40@2x-1.png", "size": 80},
        {"name": "Icon-App-40x40@2x.png", "size": 80},
        {"name": "Icon-App-40x40@3x.png", "size": 120},
        {"name": "Icon-App-60x60@2x.png", "size": 120},
        {"name": "Icon-App-60x60@3x.png", "size": 180},
        {"name": "Icon-App-76x76@1x.png", "size": 76},
        {"name": "Icon-App-76x76@2x.png", "size": 152},
        {"name": "Icon-App-83.5x83.5@2x.png", "size": 167},
        {"name": "ItunesArtwork@2x.png", "size": 1024},
    ]
    out_dir = join(output_dir, "app", "appicon", "ios", "AppIcon.appiconset")
    _mkdirs(out_dir)
    for i in icons:
        args = [
            inkscape,
            "-z",
            "-o", join(out_dir, i["name"]),
            "-w", str(i["size"]),
            "-h", str(i["size"]),
            icon
        ]
        check_call(args)

    # Create Icons for Android:
    icons = [
        {"name": "mipmap-hdpi/ic_launcher.png", "size": 72},
        {"name": "playstore-icon.png", "size": 512},
        {"name": "mipmap-xxhdpi/ic_launcher.png", "size": 144},
        {"name": "mipmap-ldpi/ic_launcher.png", "size": 36},
        {"name": "mipmap-xxxhdpi/ic_launcher.png", "size": 192},
        {"name": "mipmap-xhdpi/ic_launcher.png", "size": 96},
        {"name": "mipmap-mdpi/ic_launcher.png", "size": 48},
    ]
    for i in icons:
        out_file = join(output_dir, "app", "appicon", "android", i["name"])
        out_dir = dirname(out_file)
        _mkdirs(out_dir)
        args = [
            inkscape,
            "-z",
            "-o", out_file,
            "-w", str(i["size"]),
            "-h", str(i["size"]),
            icon
        ]
        check_call(args)

    # Create Badge Icon for Android:
    icons = [
        {"name": "mipmap-hdpi/badge.png", "size": 72},
        {"name": "mipmap-xxhdpi/badge.png", "size": 144},
        {"name": "mipmap-ldpi/badge.png", "size": 36},
        {"name": "mipmap-xxxhdpi/badge.png", "size": 192},
        {"name": "mipmap-xhdpi/badge.png", "size": 96},
        {"name": "mipmap-mdpi/badge.png", "size": 48},
    ]
    for i in icons:
        out_file = join(output_dir, "app", "appicon", "android", i["name"])
        out_dir = dirname(out_file)
        _mkdirs(out_dir)
        args = [
            inkscape,
            "-z",
            "-o", out_file,
            "-w", str(i["size"]),
            "-h", str(i["size"]),
            icon_transparent
        ]
        check_call(args)

    # Create Small Badge Icon for Android (usually monochrome display):
    icons = [
        {"name": "mipmap-hdpi/icon.png", "size": 72},
        {"name": "mipmap-xxhdpi/icon.png", "size": 144},
        {"name": "mipmap-ldpi/icon.png", "size": 36},
        {"name": "mipmap-xxxhdpi/icon.png", "size": 192},
        {"name": "mipmap-xhdpi/icon.png", "size": 96},
        {"name": "mipmap-mdpi/icon.png", "size": 48},
    ]
    for i in icons:
        out_file = join(output_dir, "app", "appicon", "android", i["name"])
        out_dir = dirname(out_file)
        _mkdirs(out_dir)
        args = [
            inkscape,
            "-z",
            "-o", out_file,
            "-w", str(i["size"]),
            "-h", str(i["size"]),
            icon_badge
        ]
        check_call(args)

    # Create icons for Desktop
    for i in [16, 32, 64, 128, 256]:
        out_dir = join(
            output_dir,
            "app", "icons", "hicolor", f"{i}x{i}", "apps"
        )
        _mkdirs(out_dir)
        out_file = join(out_dir, "net.rpdev.OpenTodoList.png")
        args = [
            inkscape,
            "-z",
            "-o", out_file,
            "-w", str(i),
            "-h", str(i),
            icon_transparent
        ]
        check_call(args)

    # Create Windows Icon file:
    out_dir = join(output_dir, "app", "res")
    _mkdirs(out_dir)
    out_file = join(out_dir, "OpenTodoList.ico")
    args = [
        "convert",
        "-background", "none",
        icon_transparent,
        "-define", "icon:auto-resize",
        out_file
    ]
    check_call(args)

    # Create macOS iconset (note: needs post-processing on
    # macOS due to lack of iconutil):
    out_dir = join(
        output_dir, "app", "appicon", "macos", "OpenTodoList.iconset"
    )
    _mkdirs(out_dir)
    icons = [
        {"name": "icon_16x16.png", "size": 16},
        {"name": "icon_16x16@2x.png", "size": 32},
        {"name": "icon_32x32.png", "size": 32},
        {"name": "icon_32x32@2x.png", "size": 64},
        {"name": "icon_128x128.png", "size": 128},
        {"name": "icon_128x128@2x.png", "size": 256},
        {"name": "icon_256x256.png", "size": 256},
        {"name": "icon_256x256@2x.png", "size": 512},
        {"name": "icon_512x512.png", "size": 512},
        {"name": "icon_512x512@2x.png", "size": 1024},
    ]
    for i in icons:
        out_file = join(out_dir, i["name"])
        args = [
            inkscape,
            "-z",
            "-o", out_file,
            "-w", str(i["size"]),
            "-h", str(i["size"]),
            icon_transparent
        ]
        check_call(args)


if __name__ == "__main__":
    from argparse import ArgumentParser
    from os.path import basename
    from os.path import dirname
    from os.path import join

    inkscape = "inkscape"
    icon = join(
        dirname(basename(__file__)),
        "assets", "OpenTodoList-Opaque.svg"
    )
    icon_transparent = join(
        dirname(basename(__file__)),
        "assets", "OpenTodoList-Transparent.svg"
    )
    icon_badge = join(
        dirname(basename(__file__)),
        "assets", "OpenTodoList-Icon.svg"
    )
    output_dir = join(
        dirname(basename(__file__))
    )

    parser = ArgumentParser(
        description="Create the AppIcon"
    )
    parser.add_argument(
        "--inkscape", "-I",
        help="Path to Inkscape. Defaults to {}".format(inkscape),
        default=inkscape
    )
    parser.add_argument(
        "--icon", "-i",
        help="Path to icon file. Defaults to {}".format(icon),
        default=icon
    )
    parser.add_argument(
        "--icon-transparent", "-t",
        help="Path to icon file with transparent background. "
             "Defaults to {}".format(icon_transparent),
        default=icon_transparent
    )
    parser.add_argument(
        "--icon-badge", "-b",
        help="Path to small/badge icon file. "
             "Defaults to {}".format(icon_transparent),
        default=icon_badge
    )
    parser.add_argument(
        "--output", "-o",
        help="Path to output directory. Defaults to {}".format(output_dir),
        default=output_dir
    )
    args = parser.parse_args()
    mk_appicon(
        args.icon, args.icon_transparent, args.icon_badge, args.output, inkscape=args.inkscape)
