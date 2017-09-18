
function documentFormatterForItem(item) {
    while (item !== undefined && item !== null) {
        var formatter = item["documentFormatter"];
        if (formatter !== undefined) {
            return formatter;
        }
        item = item.parent;
    }
}

function richTextEditorForItem(item) {
    while (item !== undefined && item !== null) {
        if (item["isRichTextEditor"] !== undefined) {
            return item;
        }
        item = item.parent;
    }
    return undefined;
}

function checkCap(item, action) {
    while (item !== undefined && item !== null) {
        if (typeof(item[action]) === "function") {
            return item;
        }
        item = item.parent;
    }
    return false;
}

function execCap(item, action) {
    while (item !== undefined && item !== null) {
        if (typeof(item[action]) === "function") {
            return item[action]();
        }
        item = item.parent;
    }
}

function viewLibrary(stack, library, tag, component) {
    stack.clear();
    if (library) {
        stack.push(component, { library: library, tag: tag });
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

function parentItemHavingFunction(item, fun) {
    var result = item;
    while (result) {
        if (typeof(result[fun]) === "function") {
            break;
        }
        result = result.parent;
    }
    return result;
}
