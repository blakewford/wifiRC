package org.starlo.remote;

import android.os.*;
import android.app.*;
import android.text.*;
import android.widget.*;
import android.net.wifi.*;
import android.text.format.*;
import android.content.res.*;

import java.io.*;
import java.net.*;

public class MainActivity extends Activity implements Runnable
{
    public static final int PORT = 8080;
    public static final int STEERING_DEFAULT = 50;
    public static final int ACCELERATOR_DEFAULT = 0;

    public static final int IDLE = 0;
    public static final int FORWARD = 1;
    public static final int REVERSE = 2;
    public static final int LEFT = 4;
    public static final int RIGHT = 8;

    public static final String COMMAND_NAMES[] = {"IDLE","FORWARD","REVERSE","INVALID","LEFT","INVALID","INVALID","INVALID","RIGHT"};

    private static final String HEADER =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Response>\n";
    private static final String FOOTER = "</Response>";

    private static final String JSON_HEADER = "{\n \"name\": \"_wifiRC.set\",\n \"parameters\": {\n";
    private static final String JSON_FOOTER = " }\n}\n";

    private final static String CONTENT_LENGTH_KEY = "Content-Length: ";

    private SeekBar mAccelBar = null;
    private SeekBar mSteeringBar = null;

    private ToggleButton mLightsButton = null;
    private ToggleButton mReverseButton = null;

    private TextView mDeviceName = null;

    private boolean mIsRunning = false;
    private ServerSocket mServerSocket = null;

    private boolean mVehiclePosted = false;

    private enum Content
    {
        PLATFORM
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        WifiManager manager = (WifiManager) getSystemService(WIFI_SERVICE);
        ((TextView)findViewById(R.id.ip_address)).setText(Formatter.formatIpAddress(manager.getConnectionInfo().getIpAddress()));

        mAccelBar = (SeekBar)findViewById(R.id.accelerator);
        mSteeringBar = (SeekBar)findViewById(R.id.steering);
        mAccelBar.setProgress(ACCELERATOR_DEFAULT);
        mAccelBar.setOnSeekBarChangeListener(new Listener(ACCELERATOR_DEFAULT));
        mSteeringBar.setProgress(STEERING_DEFAULT);
        mSteeringBar.setOnSeekBarChangeListener(new Listener(STEERING_DEFAULT));

        mLightsButton = (ToggleButton)findViewById(R.id.lights_enabled);
        mReverseButton = (ToggleButton)findViewById(R.id.reverse_engaged);

        mDeviceName = (TextView)findViewById(R.id.device_type);

        start();
    }

    private class Listener implements SeekBar.OnSeekBarChangeListener
    {
        private int mDefault = 0;

        public Listener(int defaultValue)
        {
            mDefault = defaultValue;
        }

        public void onStartTrackingTouch(SeekBar seekBar)
        {
        }

        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
        {
        }

        public void onStopTrackingTouch(SeekBar seekBar)
        {
            seekBar.setProgress(mDefault);
        }
    }

// Everything below this line is largely pulled from SimpleWebServer.java
// and is subject to the following license.

/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

    public void start()
    {
        Resources res = getResources();
        new ResetTask().execute(res.getString(R.string.device_label));
        mIsRunning = true;
        new Thread(this).start();
    }

    public void stop()
    {
        try
        {
            mIsRunning = false;
            if(null != mServerSocket)
            {
                mServerSocket.close();
                mServerSocket = null;
            }
        }catch(IOException e)
        {
        }
    }

    @Override
    public void run()
    {
        try
        {
            mServerSocket = new ServerSocket(PORT);
            while(mIsRunning)
            {
                Socket socket = mServerSocket.accept();
                handle(socket);
                socket.close();
            }
        }catch(Exception e)
        {
        }
    }

    private void handle(Socket socket) throws IOException
    {
        BufferedReader reader = null;
        PrintStream output = null;
        try
        {
            String route = null;

            // Read HTTP headers and parse out the route.
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String line;
            Integer contentLength = 0;
            while(!TextUtils.isEmpty(line = reader.readLine()))
            {
                if(line.startsWith("GET /"))
                {
                    int start = line.indexOf('/') + 1;
                    int end = line.indexOf(' ', start);
                    route = line.substring(start, end);
                    break;
                }
                if(line.startsWith("POST /"))
                {
                    mVehiclePosted = true;
                    while(!line.startsWith(CONTENT_LENGTH_KEY))
                    {
                        line = reader.readLine();
                    }
                    contentLength = Integer.parseInt(line.substring(CONTENT_LENGTH_KEY.length()));
                }
            }

            // Output stream that we send the response to
            output = new PrintStream(socket.getOutputStream());

            byte[] bytes = new byte[0];
            boolean setContent = contentLength > 0;
            if(setContent)
            {
                StringBuilder builder = new StringBuilder();
                for(int i = 0; i < contentLength; i++)
                {
                    builder.append(Character.toChars(reader.read()));
                }

                final Integer id;
                String pair = builder.toString();
                int delimiterIndex = pair.indexOf(':');
                Content content = Content.valueOf(pair.substring(0, delimiterIndex));
                switch(content)
                {
                    case PLATFORM:
                        id = R.id.device_type;
                        break;
                    default:
                        id = null;
                }
                final String post = pair.substring(delimiterIndex+1);
                runOnUiThread(new Runnable(){
                    public void run()
                    {
                        ((TextView)findViewById(id)).setText(post);
                    }
                });
            }
            else
            {
                // Prepare the content to send.
                if(null == route)
                {
                    return;
                }
                bytes = loadContent(route);
                if(null == bytes)
                {
                    return;
                }
            }

            // Send out the content.
            output.println("HTTP/1.0 200 OK");
            output.println("Content-Type: ");
            output.println("Content-Length: " + bytes.length);
            output.println();
            output.write(bytes);
            output.flush();
        }
        finally
        {
            if(null != output) output.close();
            if(null != reader) reader.close();
        }
    }

    private byte[] buildTerminal(String name, String value)
    {
        StringBuilder builder = new StringBuilder();
        builder.append("<Terminal><Name>");
        builder.append(name);
        builder.append("</Name><Value>");
        builder.append(value);
        builder.append("</Value></Terminal>\n");
        byte[] bytes = new byte[0];
        try
        {
            bytes = builder.toString().getBytes("UTF-8");
        }catch(Exception e)
        {
        }

        return bytes;
    }

    private byte[] buildJsonTerminal(String name, String value, boolean last)
    {
        StringBuilder builder = new StringBuilder();
        builder.append("\"_");
        builder.append(name);
        builder.append("\": \"");
        builder.append(value);
        if(last)
        {
            builder.append("\"\n");
        }
        else
        {
            builder.append("\",\n");
        }
        byte[] bytes = new byte[0];
        try
        {
            bytes = builder.toString().getBytes("UTF-8");
        }catch(Exception e)
        {
        }

        return bytes;
    }

    private byte[] loadContent(String fileName) throws IOException
    {
        boolean xmlCommand = fileName.equals("CommandServer/currentCommand");

        int accelProgress = mAccelBar.getProgress();
        int directionState = mReverseButton.isChecked() ? REVERSE: FORWARD;
        int direction = accelProgress > ACCELERATOR_DEFAULT ? directionState: IDLE;
        int steeringProgress = mSteeringBar.getProgress();
        if(steeringProgress != STEERING_DEFAULT)
        {
            direction |= (steeringProgress > (STEERING_DEFAULT/2)) ? RIGHT: LEFT;
        }

        int stringIndex = direction;
        if(stringIndex > 3) stringIndex &= 0xC;

        ByteArrayOutputStream output = new ByteArrayOutputStream();

        if(xmlCommand)
        {
            output.write(HEADER.getBytes());
            output.write(buildTerminal("direction", Long.valueOf(direction).toString()));
            output.write(buildTerminal("directionString", COMMAND_NAMES[stringIndex]));
            output.write(buildTerminal("magnitude", Long.valueOf(accelProgress).toString()));
            output.write(buildTerminal("lights", Long.valueOf(mLightsButton.isChecked() ? 1: 0).toString()));
            output.write(buildTerminal("gear", Long.valueOf(directionState == REVERSE ? 1: 0).toString()));
            output.write(FOOTER.getBytes());
        }
        else
        {
            output.write(JSON_HEADER.getBytes());
            output.write(buildJsonTerminal("direction", Long.valueOf(direction).toString(), false));
            output.write(buildJsonTerminal("directionString", COMMAND_NAMES[stringIndex], false));
            output.write(buildJsonTerminal("magnitude", Long.valueOf(accelProgress).toString(), false));
            output.write(buildJsonTerminal("lights", Long.valueOf(mLightsButton.isChecked() ? 1: 0).toString(), false));
            output.write(buildJsonTerminal("gear", Long.valueOf(directionState == REVERSE ? 1: 0).toString(), true));
            output.write(JSON_FOOTER.getBytes());
        }
        output.flush();

        return output.toByteArray();
    }

    private class ResetTask extends AsyncTask<String,Void,Void>
    {
        private String mDefaultDeviceName = null;

        public Void doInBackground(String... params)
        {
            mDefaultDeviceName = params[0];
            try
            {
                Thread.sleep(1300);
            }catch(Exception e)
            {
            }

            return null;
        }

        public void onPostExecute(Void result)
        {
            if(mVehiclePosted)
            {
                mVehiclePosted = false;
            }
            else
            {
                mDeviceName.setText(mDefaultDeviceName);
            }

            new ResetTask().execute(mDefaultDeviceName);
        }
    }

}
