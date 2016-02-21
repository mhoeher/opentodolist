
function createNote(library, edit) {
    library.addNote(edit.displayText);
    edit.text = "";
    edit.focus = false;
}

function numberOfColumns(page) {
    var minWidth = Math.max(defaultFontPixelSize, 5) * 30;
    var result = page.width / minWidth;
    result = Math.ceil(result);
    result = Math.max(result, 1);
    return result;
}

function sizeOfColumns(page) {
    return page.width / numberOfColumns(page);
}
