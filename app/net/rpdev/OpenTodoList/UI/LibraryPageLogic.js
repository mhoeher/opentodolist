
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
    function includes(text, word) {
        if (text !== undefined) {
            return text.toString().toLowerCase().indexOf(word) >= 0;
        }
        return false;
    }
    var testItem = function(item, word) {
        return includes(item.title, word) || includes(item.notes, word);
    }
    try {
        for (var i = 0; i < words.length; ++i) {
            var word = words[i];
            if (word === "") {
                continue;
            }

            if (testItem(item, word)) {
                return true;
            } else if (item.itemType === "TodoList") {
                for (var j = 0; j < item.todos.length; ++j) {
                    var todo = item.todos[j];
                    if (testItem(todo, word)) {
                        return true;
                    } else {
                        for (var k = 0; k < todo.tasks.length; ++k) {
                            var task = todo.tasks[k];
                            if (testItem(task, word)) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    } catch (e) { console.error(e); }
    return false;
}
