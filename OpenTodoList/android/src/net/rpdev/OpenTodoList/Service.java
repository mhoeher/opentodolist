package net.rpdev.OpenTodoList;

import android.os.Binder;
import android.os.IBinder;
import android.content.Intent;
import android.util.Log;

public class Service extends android.app.Service {

  private Service m_instance;
  private static final String TAG = "OpenTodoList.Service";

  public Service()
  {
    Log.w( TAG, "Creating OpenTodoList service" );
    m_instance = this;
  }

  public class ServiceBinder extends Binder {
    Service getService() {
      return Service.this;
    }
  }

  @Override
  public void onCreate() {
    Log.w( TAG, "OpenTodoList Service onCreate called" );
    //TODO: Ensure C++ service is started
  }

  @Override
  public int onStartCommand(Intent intend, int flags, int startId) {
    Log.w( TAG, "OpenTodoList Service onStartCommand called" );
    return START_STICKY;
  }

  @Override
  public void onDestroy() {
    Log.w( TAG, "OpenTodoList Service onDestroy called" );
    //TODO: Shut down C++ service? Or rather rely on the process being killed next?
  }

  @Override
  public IBinder onBind(Intent intent) {
    return m_binder;
  }

  private static final IBinder m_binder = new Binder();

}

