package net.rpdev.OpenTodoList;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class QtBroadcastReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        Intent startServiceIntent = new Intent(context, QtBroadcastReceiver.class);
        context.startService(startServiceIntent);
    }
}
