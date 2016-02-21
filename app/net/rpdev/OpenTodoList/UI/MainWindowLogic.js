
function documentFormatterForItem(item) {
    while (item !== undefined && item !== null) {
        var formatter = item["documentFormatter"];
        if (formatter !== undefined) {
            return formatter;
        }
        item = item.parent;
    }
}

function viewLibrary(stack, library, component) {
    stack.clear();
    if (library) {
        stack.push({item: component, properties: { library: library}});
    }
}

function cancelCurrent(stack) {
    var currentItem = stack.currentItem;
    if (typeof(currentItem["cancel"]) === "function") {
        currentItem.cancel();
    }
    var result = stack.pop();
}

function saveCurrent(stack) {
    var currentItem = stack.currentItem;
    currentItem.save();
    stack.pop();
}

function goBack(stack, event) {
    if (stack.currentItem) {
        cancelCurrent(stack);
        event.accepted = true;
    } else {
        event.accepted = false;
    }
}
