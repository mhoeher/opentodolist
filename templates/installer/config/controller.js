function Controller()
{
    if (installer.value("os") === "win") {
        var programFiles = installer.environmentVariable("ProgramFiles");
        if (programFiles !== "") {
            installer.setValue("TargetDir", programFiles + "/OpenTodoList");
        }
    }
}
