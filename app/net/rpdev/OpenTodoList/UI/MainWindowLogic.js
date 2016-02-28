
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

function goBack(stack) {
    if (stack.depth > 1) {
        cancelCurrent(stack);
    } else {
        if (Qt.platform.os === "android") {
            Qt.quit();
        }
    }
}

function viewPage(stack, component) {
    stack.clear();
    stack.push({item: component, properties: { stack: stack }});    
}
