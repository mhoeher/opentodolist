#!/bin/env python3

"""Set the target SDK version in Android deployment config file."""

import json
from argparse import ArgumentParser
from copy import deepcopy
from os.path import dirname, join


def _main():
    parser = ArgumentParser(usage="Set Android target SDK version in deployment config")
    parser.add_argument(
        "deployment_settings", help="Path to the deployment settings JSON file."
    )
    parser.add_argument(
        "target_sdk", type=str, help="Target SDK version to build against."
    )
    parser.add_argument(
        "--android-package-source-directory",
        type=str,
        help="Override the Android package source directory.",
    )
    args = parser.parse_args()
    _patch_deployment_cfg(
        args.deployment_settings, args.target_sdk, args.android_package_source_directory
    )


def _patch_deployment_cfg(path, version, android_package_source_directory):
    with open(path, "r") as cfg_file:
        config = json.load(cfg_file)
    config["android-target-sdk-version"] = version
    if android_package_source_directory is not None:
        config["android-package-source-directory"] = android_package_source_directory
    with open(path, "w") as cfg_file:
        json.dump(config, cfg_file, indent=4)


if __name__ == "__main__":
    _main()
