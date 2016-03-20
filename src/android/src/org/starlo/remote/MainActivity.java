package org.starlo.remote;

import android.os.Bundle;
import android.app.Activity;
import android.widget.*;
import android.net.wifi.WifiManager;
import android.text.format.Formatter;

public class MainActivity extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        WifiManager manager = (WifiManager) getSystemService(WIFI_SERVICE);
        ((TextView)findViewById(R.id.ip_address)).setText(Formatter.formatIpAddress(manager.getConnectionInfo().getIpAddress()));
        new SimpleWebServer(8080, getAssets()).start();
    }
}
