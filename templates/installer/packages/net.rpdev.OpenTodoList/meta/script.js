function Component()
{
    // constructor
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();

        // on Windows, create shortcuts in start menu dir
        if (installer.value("os") === "win") {
            component.addOperation("CreateShortcut",
                                   "@TargetDir@/OpenTodoList/bin/OpenTodoList.exe",
                                   "@StartMenuDir@/OpenTodoList.lnk");
        }

        // on Unix, create a desktop file:
        if (installer.value("os") === "x11") {
            component.addOperation("CreateDesktopEntry",
                                   "OpenTodoList.desktop",
                                   "Name=OpenTodoList\n" +
                                   "Type=Application\n" +
                                   "Exec=@TargetDir@/OpenTodoList/bin/OpenTodoList\n" +
                                   "Icon=@TargetDir@/OpenTodoList/share/OpenTodoList/icons/OpenTodoList80.png\n" +
                                   "Terminal=false\n" +
                                   "Categories=Utility;");
        }

        // todo: What should we do on Mac?
        if (installer.value("os") === "mac") {
        }

    } catch (e) {
        print(e);
    }
}
