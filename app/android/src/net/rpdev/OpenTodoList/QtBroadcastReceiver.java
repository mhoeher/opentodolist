package net.rpdev.OpenTodoList;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.util.Log;

import org.qtproject.qt.android.bindings.QtActivity;

public class QtBroadcastReceiver extends BroadcastReceiver {

    private static final String TAG = "OpenTodoList.BCRecv";

    // Quit the service + application
    public static String ACTION_QUIT = "OpenTodoList.Quit";

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.i(TAG, "Processing intent " + intent.toString());
        String action = intent.getAction();
        if (action != null) {
            Log.i(TAG, "Got intent action" + action);
            if (action.equals(ACTION_QUIT)) {
                Log.i(TAG, "Received request to stop service");
                Intent stopServiceIntent = new Intent(context, BackgroundService.class);
                context.stopService(stopServiceIntent);
                return;
            }
        }

        Log.i(TAG, "Received request to start background service");
        Intent startServiceIntent = new Intent(context, BackgroundService.class);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            context.startForegroundService(startServiceIntent);
        } else {
            context.startService(startServiceIntent);
        }
    }
}
