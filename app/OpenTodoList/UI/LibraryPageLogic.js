
function createNote(library, edit) {
    var result = library.addNote();
    result.title = edit.displayText;
    edit.text = "";
    edit.focus = false;
    return result;
}

function createTodoList(library, edit) {
    var result = library.addTodoList();
    result.title = edit.displayText;
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
    if (correction === undefined) {
        correction = 0;
    }
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
    function exceptionToString(exception) {
        for (var member in exception) {
            console.debug(member + ": " + excdeption[member]);
        }
    }


    try {
        if (testItem(item, words)) {
            return true;
        } else if (item.itemType === "TodoList") {
            var todos = item.library.todos;
            for (var j = 0; j < todos.count; ++j) {
                var todo = todos.get(j);
                if (todo === null) {
                    continue;
                }
                if (todo.todoListUid === item.uid && testItem(todo, words)) {
                    return true;
                } else {
                    var tasks = item.library.tasks;
                    for (var k = 0; k < tasks.count; ++k) {
                        var task = tasks.get(k);
                        if (task === null) {
                            continue;
                        }
                        if (task.todoUid === todo.uid && testItem(task, words)) {
                            return true;
                        }
                    }
                }
            }
        } else if (item.itemType === "Todo") {
            tasks = item.library.tasks;
            for (j = 0; j < tasks.count; ++j) {
                task = tasks.get(j);
                if (task === null) {
                    continue;
                }
                if (task.todoUid === item.uid && testItem(task, words)) {
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
