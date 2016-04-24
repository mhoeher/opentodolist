
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

function itemMatchesFilter(item, filter) {
    var words = filter.toLowerCase().split(/\s*,\s*/);
    function includes(text, words) {
        if (text !== undefined) {
            var lc = text.toString().toLowerCase();
            for (var i = 0; i < words.length; ++i) {
                var word = words[i];
                if (word !== "") {
                    if (lc.indexOf(word) >= 0) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    function testItem(item, word) {
        return includes(item.title, word) || includes(item.notes, word);
    }
    try {
        if (testItem(item, words)) {
            return true;
        } else if (item.itemType === "TodoList") {
            for (var j = 0; j < item.todos.length; ++j) {
                var todo = item.todos[j];
                if (testItem(todo, words)) {
                    return true;
                } else {
                    for (var k = 0; k < todo.tasks.length; ++k) {
                        var task = todo.tasks[k];
                        if (testItem(task, words)) {
                            return true;
                        }
                    }
                }
            }
        } else if (item.itemType === "Todo") {
            for (j = 0; j < item.tasks.length; ++j) {
                if (testItem(item.tasks[j], words)) {
                    return true;
                }
            }
        }
    } catch (e) { console.error("Exception in LibraryPageLogic.itemMatchesFilter: " + e); }
    return false;
}

function itemMatchesFilterWithDefault(item, filter, defaultResult) {
    if ((filter === undefined) || (filter === null) || (filter === "")) {
        return defaultResult;
    }
    return itemMatchesFilter(item, filter);
}
