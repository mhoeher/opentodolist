#!/bin/env python3

"""
Prepare deployment settings for Android APK build.

For Android, we build for all supported architectures in one go. To deliver architecture specific
APKs, we just take architecture specific build files and created a dedicated deployment 
configuration for each, so we can create stripped down APKs.
"""

import json
from argparse import ArgumentParser
from copy import deepcopy
from os.path import dirname, join


def _main():
    parser = ArgumentParser(usage="Prepare architecture specific APK builds")
    parser.add_argument(
        "deployment_settings", help="Path to the deployment settings JSON file."
    )
    args = parser.parse_args()
    _split_deployment_config(args.deployment_settings)


def _split_deployment_config(path):
    with open(path, "r") as cfg_file:
        config = json.load(cfg_file)
        archs = config["architectures"]
        dir_path = dirname(path)
        for arch in archs:
            config_copy = deepcopy(config)
            config_copy["architectures"] = {
                arch: archs[arch]
            }
            new_cfg_path = join(dir_path, "android_deployment_settings_{}.json".format(arch))
            with open(new_cfg_path, "w") as new_cfg_file:
                json.dump(config_copy, new_cfg_file, indent=4)


if __name__ == "__main__":
    _main()
