package net.rpdev.OpenTodoList;

import android.content.Intent;
import android.net.Uri;
import androidx.core.content.FileProvider;

import org.qtproject.qt.android.bindings.QtActivity;

import java.util.LinkedList;
import java.util.List;
import java.io.File;

public class MainActivity extends QtActivity
{
    private List<String> m_pendingAppLinks = new LinkedList<String>();

    @Override
    protected void onNewIntent(Intent intent) {
        if ("android.intent.action.VIEW".equals(intent.getAction())) {
            if ("opentodolist".equals(intent.getData().getScheme())) {
                // This is an app-internal link. Save it for later - the app will
                // collect it once the C++ part becomes active again.
                addPendingAppLink(intent.getData().toString());
                return;
            }
        }
        super.onNewIntent(intent);
    }

    /**
     * @brief Get the number of pending app links we received.
     */
    public synchronized int getPendingAppLinksCount() {
        return m_pendingAppLinks.size();
    }

    /**
     * @brief Get a pending app link.
     *
     * This returns a pending app link by its index.
     *
     * @param index The index of the URL to return-
     */
    public synchronized String getPendingAppLink(int index) {
        if (index >= 0 && index < m_pendingAppLinks.size()) {
            return m_pendingAppLinks.get(index);
        }
        return null;
    }

    /**
     * @brief Clear the pending app links.
     */
    public synchronized void clearPendingAppLinks() {
        m_pendingAppLinks.clear();
    }

    /**
     * @brief Appends a pending app link.
     *
     * @param link The link to append to the list.
     */
    public synchronized void addPendingAppLink(String link) {
        if (link != null) {
            m_pendingAppLinks.add(link);
        }
    }

    /**
     * @brief Open a file in a suitable app.
     *
     * @param path The path to the file to open.
     * @return True if opening was successful or false otherwise.
     */
    public boolean openFile(String path) {
        File file = new File(path);

        Uri uri = FileProvider.getUriForFile(this, "net.rpdev.opentodolist", file);
        String mimeType = getContentResolver().getType(uri);

        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_VIEW);
        intent.setDataAndType(uri, mimeType);
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        startActivity(intent);
        return true;
    }
}
