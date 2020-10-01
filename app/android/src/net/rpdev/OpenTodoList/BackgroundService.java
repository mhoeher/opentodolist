package net.rpdev.OpenTodoList;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.NotificationManagerCompat;
import android.util.Log;
import android.content.Context;
import android.content.Intent;

import net.rpdev.opentodolist.R;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtService;

public class BackgroundService extends QtService
{
    private static final String TAG = "OpenTodoList.BgService";
    private static NotificationManager m_notificationManager;
    private static NotificationCompat.Builder m_notificationBuilder;
    private static String CHANNEL_ID = "OpenTodoList";

    // Quit the service + application
    public static String ACTION_QUIT = "OpenTodoList.Quit";

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Creating OpenTodoList background service");
        Notification notification = showForegroundNotification(getApplicationContext(), 1);
        startForeground(1, notification);
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "Destroying OpenTodoList background service");
        super.onDestroy();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "Processing start command in service");
        int ret = super.onStartCommand(intent, flags, startId);
        return ret;
    }

    public static void startQtAndroidService(Context context) {
        Log.i(TAG, "Starting OpenTodoList Background Service");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            context.startForegroundService(new Intent(context, BackgroundService.class));
        } else {
            context.startService(new Intent(context, BackgroundService.class));
        }
    }

    private Notification showForegroundNotification(Context context, int notificationId) {
        if (m_notificationManager == null) {
            m_notificationManager = (NotificationManager) context.getSystemService(
                    Context.NOTIFICATION_SERVICE);
        }
        if (m_notificationManager == null) {
            Log.w(TAG, "Failed to get NotificationManager");
            return null;
        }
        if (m_notificationBuilder == null) {
            if (Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {

                int importance = NotificationManager.IMPORTANCE_LOW;
                NotificationChannel notificationChannel = new NotificationChannel(
                        CHANNEL_ID, "OpenTodoList", importance);
                m_notificationManager.createNotificationChannel(notificationChannel);
            }
            m_notificationBuilder = new NotificationCompat.Builder(
                    context, CHANNEL_ID);
        }

        m_notificationBuilder
                .setSmallIcon(R.mipmap.icon)
                .setLargeIcon(BitmapFactory.decodeResource(
                        context.getResources(), R.mipmap.badge))
                .setContentTitle("Background Sync")
                .setContentText("App continues to sync your data in the background")
                .setPriority(NotificationManagerCompat.IMPORTANCE_LOW)
                .setColor(Color.DKGRAY);

        // Open app when activity is interacted with:
        Intent notifyIntent = new Intent(this, QtActivity.class);
        PendingIntent notifyPendingIntent = PendingIntent.getActivity(
                this, 0, notifyIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        m_notificationBuilder.setContentIntent(notifyPendingIntent);

        // Let user quit the app+service:
        Intent quitAppIntent = new Intent(context, QtBroadcastReceiver.class);
        quitAppIntent.setAction(QtBroadcastReceiver.ACTION_QUIT);
        PendingIntent pendingQuitAppIntent = PendingIntent.getBroadcast(
                context, 0, quitAppIntent, 0);
        m_notificationBuilder.addAction(R.mipmap.icon, "Quit", pendingQuitAppIntent);

        Notification notification = m_notificationBuilder.build();
        m_notificationManager.notify(notificationId, notification);
        return notification;
    }
}
