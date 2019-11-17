#!/usr/bin/env python

def mk_appicon(icon, output_dir, inkscape=None):
    from os import makedirs
    from os.path import join
    from os.path import abspath
    from subprocess import check_call
    if inkscape is None:
        inkscape = "inkscape"
    try:
        makedirs(output_dir)
    except OSError:
        pass # Ok, directory exists
    icon = abspath(icon)
    output_dir = abspath(output_dir)
    for s in [29, 40, 50, 57, 60, 72, 76]:
        # Single size icon:
        args = [
            inkscape,
            "-z",
            "-e", join(output_dir, "AppIcon{s}x{s}.png".format(s=s)),
            "-w", str(s),
            "-h", str(s),
            icon
        ]
        check_call(args)

        # Single size icon for iPad:
        args = [
            inkscape,
            "-z",
            "-e", join(output_dir, "AppIcon{s}x{s}~ipad.png".format(s=s)),
            "-w", str(s),
            "-h", str(s),
            icon
        ]
        check_call(args)

        # Double size icon:
        args = [
            inkscape,
            "-z",
            "-e", join(output_dir, "AppIcon{s}x{s}@2x.png".format(s=s)),
            "-w", str(s*2),
            "-h", str(s*2),
            icon
        ]
        check_call(args)

        # Double size icon for iPad:
        args = [
            inkscape,
            "-z",
            "-e", join(output_dir, "AppIcon{s}x{s}@2x~ipad.png".format(s=s)),
            "-w", str(s*2),
            "-h", str(s*2),
            icon
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
        "templates", "svg", "OpenTodoList.svg"
    )
    output_dir = join(
        dirname(basename(__file__)),
        "app", "ios", "AppIcon"
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
        "--output", "-o",
        help="Path to output directory. Defaults to {}".format(output_dir),
        default=output_dir
    )
    args = parser.parse_args()
    mk_appicon(args.icon, args.output, inkscape=args.inkscape)
