package net.rpdev.OpenTodoList;

import android.util.Log;
import android.content.Context;
import android.content.Intent;
import org.qtproject.qt5.android.bindings.QtService;

public class BackgroundService extends QtService
{
    private static final String TAG = "OpenTodoList.BackgroundService";

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Creating OpenTodoList background service");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "Destroying OpenTodoList background service");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        int ret = super.onStartCommand(intent, flags, startId);
        Log.i(TAG, "Processing start command in service");
        return ret;
    }

    public static void startQtAndroidService(Context context) {
        Log.i(TAG, "Starting OpenTodoList Background Service");
        context.startService(new Intent(context, BackgroundService.class));
    }
}
