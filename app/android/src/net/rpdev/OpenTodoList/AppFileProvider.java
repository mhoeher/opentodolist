package net.rpdev.OpenTodoList;

import net.rpdev.opentodolist.R;

import androidx.core.content.FileProvider;

public class AppFileProvider extends FileProvider {
    public AppFileProvider() {
        super(R.xml.file_paths);
    }
}
