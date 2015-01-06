package net.rpdev.OpenTodoList;

import android.content.Intent;
import android.os.Bundle;

public class Activity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static Activity m_instance;

    public Activity()
    {
      m_instance = this;
    }

    private void doStartService() {
      startService( new Intent( this, Service.class ) );
    }

    @Override
    public void onCreate(Bundle savedInstanceData) {
      super.onCreate(savedInstanceData);
      doStartService();
    }

    @Override
    protected void onDestroy()
    {
      //TODO: Only shut down activity, but not complete app!
      super.onDestroy();
    }

    public static void minimizeActivity() {
      if ( m_instance != null ) {
        m_instance.moveTaskToBack(true);
      }
    }

}
