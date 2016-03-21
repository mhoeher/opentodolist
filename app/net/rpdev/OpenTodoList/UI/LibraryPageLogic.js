
function createNote(library, edit) {
    var result = library.addNote(edit.displayText);
    edit.text = "";
    edit.focus = false;
    return result;
}

function createTodoList(library, edit) {
    var result = library.addTodoList(edit.displayText);
    edit.text = "";
    edit.focus = false;
    return result;
}

function numberOfColumns(page) {
    var minWidth = Math.max(defaultFontPixelSize, 5) * 30;
    var result = page.width / minWidth;
    result = Math.ceil(result);
    result = Math.max(result, 1);
    return result;
}

function sizeOfColumns(page, correction) {
    return (page.width - correction) / numberOfColumns(page);
}
