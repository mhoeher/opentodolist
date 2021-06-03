#!/bin/env python3

"""Upload terms to POEditor.com."""

import json
from fire import Fire
from pathlib import Path
from poeditor import POEditorAPI


class POEditor:
    """
    Interact with POEditor.

    This script can be used to interact with POEditor.com, e.g. uploading new
    terms or fetching translations.
    """

    _PROJECT_ID = 273303

    @classmethod
    def upload(cls, api_token: str):
        """
        Upload new terms to POEditor.com.

        This script pushes new terms from the local files to POEditor.com.
        """
        client = POEditorAPI(api_token=api_token)
        client.update_terms(
            POEditor._PROJECT_ID,
            file_path=str(
                Path(__file__).parent.parent
                / "app"
                / "translations"
                / "OpenTodoList-en.po"
            ),
        )

    @classmethod
    def download(cls, api_token: str, output_dir: str):
        client = POEditorAPI(api_token=api_token)
        langs = client.list_project_languages(POEditor._PROJECT_ID)
        output_path = Path(output_dir)
        with open(output_path / "languages.json", "w") as file:
            json.dump(langs, file, indent=4)
        for lang in langs:
            code = lang["code"]
            code_file = code.replace('-', '_')
            client.export(
                POEditor._PROJECT_ID,
                code,
                local_file=str(output_path / f"OpenTodoList-{code_file}.po"),
            )


if __name__ == "__main__":
    Fire(POEditor)
