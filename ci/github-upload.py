#!/bin/env python3

"""Simple script uploading artifacts to GitHub."""


class Uploader:
    """Upload releases to GitHub."""

    def __init__(self, user=None, project=None, tag=None, token=None,
                 files=None):
        """Create a new uploader."""
        self._api = "https://api.github.com"
        self._user = user
        self._project = project
        self._tag = tag
        self._token = token
        self._files = files
        self._upload_url = None

    def parse_args(self, argv=None):
        """Get uploader arguments from the command line."""
        from argparse import ArgumentParser
        parser = ArgumentParser(description="Upload artifacts to GitHub")
        parser.add_argument(
            "user",
            help="The GitHub username.", type=str)
        parser.add_argument(
            "project",
            help="The project name.", type=str)
        parser.add_argument(
            "tag",
            help="The tag name to upload files to.", type=str)
        parser.add_argument(
            "token",
            help="The access token.", type=str)
        parser.add_argument(
            "files",
            help="The files to upload.", type=str, nargs="+")
        args = parser.parse_args(args=argv)
        self._user = args.user
        self._project = args.project
        self._tag = args.tag
        self._token = args.token
        self._files = args.files

    def upload(self):
        """Run the upload."""
        if self._tag == "continuous":
            if self._release_exists():
                self._delete_release()
        self._create_release()
        for file in self._files:
            self._upload_file(file)

    def _release_exists(self):
        import requests
        path = "/repos/{owner}/{repo}/releases/tags/{tag}"
        path = path.format(
            owner=self._user,
            repo=self._project,
            tag=self._tag)
        headers = {
            "Authorization": "token " + self._token
        }
        resp = requests.get(self._api + path, headers=headers)
        return resp.status_code == 200

    def _delete_release(self):
        import requests
        path = "/repos/{owner}/{repo}/releases/{tag}"
        path = path.format(
            owner=self._user,
            repo=self._project,
            tag=self._tag)
        headers = {
            "Authorization": "token " + self._token
        }
        resp = requests.delete(self._api + path, headers=headers)
        if resp.status_code != 204:
            msg = "Failed to remove previous release with same name. Code: {}"
            msg = msg.format(resp.status_code)
            raise RuntimeError(msg)

    def _create_release(self):
        import json
        import requests
        path = "/repos/{user}/{project}/releases"
        path = path.format(user=self._user, project=self._project)
        data = {
            "tag_name": self._tag,
            "name": self._tag
        }
        data = json.dumps(data)
        headers = {
            "Authorization": "token " + self._token
        }
        rsp = requests.post(self._api + path, data=data, headers=headers)
        if rsp.status_code != 201:
            msg = "Failed to create release. Code: {}"
            msg = msg.format(rsp.status_code)
            raise RuntimeError(msg)
        else:
            self._upload_url = rsp.json()["upload_url"]
            if not self._upload_url.endswith("{?name,label}"):
                msg = "Unexpected upload URL: {}"
                msg = msg.format(self._upload_url)
                raise RuntimeError(msg)

    def _upload_file(self, file):
        import requests
        from os.path import basename
        from urllib.parse import quote
        base = basename(file)
        base = quote(base)
        url = self._upload_url
        url = url.replace("{?name,label}", "?name=" + base)
        with open(file, "rb") as fh:
            data = fh.read()
        headers = {
            'Content-Type': 'application/octet-stream',
            "Authorization": "token " + self._token
        }
        rsp = requests.post(url, data=data, headers=headers)
        if rsp.status_code != 201:
            msg = "Failed to upload file. Response code: {}"
            msg = msg.format(rsp.status_code)
            raise RuntimeError(msg)


def _main():
    uploader = Uploader()
    uploader.parse_args()
    uploader.upload()


if __name__ == '__main__':
    _main()
