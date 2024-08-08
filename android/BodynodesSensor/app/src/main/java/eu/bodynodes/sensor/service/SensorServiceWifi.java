/*
  MIT License

  Copyright (c) 2019-2024 Manuel Bottini

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 */

package eu.bodynodes.sensor.service;

import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ServiceInfo;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.VibrationEffect;
import android.os.Vibrator;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;
import androidx.core.app.ServiceCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.UnknownHostException;
import java.util.Timer;
import java.util.TimerTask;

import eu.bodynodes.sensor.BodynodesConstants;
import eu.bodynodes.sensor.BodynodesUtils;
import eu.bodynodes.sensor.data.AppData;
import eu.bodynodes.sensor.data.BodynodesData;
import eu.bodynodes.sensor.BodynodesProtocol;
import eu.bodynodes.sensor.R;

public class SensorServiceWifi extends Service implements SensorEventListener {

    private final static String TAG = "SensorServiceWifi";

    // DatagramSocket receive is blocking, therefore I need to encapsulate it in threads
    private Thread mDataConnectionThread = null;
    private Thread mMulticastConnectionThread = null;

    private DatagramSocket mConnector = null;
    private MulticastSocket mMulticastConnector = null;

    private String mLastDataReceived = "";
    private String mLastMulticastReceived = "";
    private String mLastMulticastIpAddress = "";
    private InetAddress mServerIpAddress = null;

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private SensorManager mSensorManager;
    private Sensor mOrientationAbsSensor;
    private Sensor mAccelerationRelSensor;
    public static final int ACCELETATION_TYPE = Sensor.TYPE_LINEAR_ACCELERATION;
    private Timer mTimer;
    private float[] mOrientationAbs;
    private float[] mAccelerationRel;
    private float[] mPrevOrientationAbs;
    private float[] mPrevAccelerationRel;

    @Override
    public void onCreate() {
        super.onCreate();
        createNotification();
        mOrientationAbs = new float[]{0, 0, 0, 0};
        mAccelerationRel = new float[]{0, 0, 0};
        mPrevOrientationAbs = new float[]{0, 0, 0, 0};
        mPrevAccelerationRel = new float[]{0, 0, 0};
        mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        AppData.setServiceRunning(true);

        Log.i(TAG,"orientation_abs_enabled = "+BodynodesData.isOrientationAbsSensorEnabled(this));
        Log.i(TAG,"acceleration_rel_enabled = "+BodynodesData.isAccelerationRelSensorEnabled(this));
        if (mSensorManager != null) {
            if(BodynodesData.isOrientationAbsSensorEnabled(this)) {
                mOrientationAbsSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
                mSensorManager.registerListener(this, mOrientationAbsSensor, AppData.getSensorIntervalMs(this) * 1000);
            }
            if(BodynodesData.isAccelerationRelSensorEnabled(this)) {
                mAccelerationRelSensor = mSensorManager.getDefaultSensor(ACCELETATION_TYPE);
                mSensorManager.registerListener(this, mAccelerationRelSensor, AppData.getSensorIntervalMs(this) * 1000);
            }
        }
        if (AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_WIFI) {
            init();
        } else {
            Log.i(TAG,"Wrong communication type for " + TAG);
            stopSelf();
        }
    }

    @SuppressLint("ForegroundServiceType")
    private void createNotification() {
        String NOTIFICATION_CHANNEL_ID = BodynodesConstants.NOTIFICATION_CHANNEL_ID;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            String channelName = BodynodesConstants.NOTIFICATION_CHANNEL_NAME;
            NotificationChannel chan = new NotificationChannel(NOTIFICATION_CHANNEL_ID, channelName, NotificationManager.IMPORTANCE_NONE);
            chan.setLightColor(Color.BLUE);
            chan.setLockscreenVisibility(Notification.VISIBILITY_PRIVATE);
            NotificationManager manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
            assert manager != null;
            manager.createNotificationChannel(chan);
            NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this, NOTIFICATION_CHANNEL_ID);
            Notification notification = notificationBuilder.setOngoing(true)
                    .setSmallIcon(R.drawable.bn_logo_topbar)
                    .setContentTitle(getText(R.string.app_name))
                    .setPriority(NotificationManager.IMPORTANCE_MIN)
                    .setCategory(Notification.CATEGORY_SERVICE)
                    .build();
            ServiceCompat.startForeground(this, BodynodesConstants.SENSOR_SERVICE_NOTIFICATION_ID, notification, ServiceInfo.FOREGROUND_SERVICE_TYPE_REMOTE_MESSAGING);
        } else {
            NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this, NOTIFICATION_CHANNEL_ID);
            Notification notification = notificationBuilder.setOngoing(true)
                    .setSmallIcon(R.drawable.bn_logo_topbar)
                    .setContentTitle(getText(R.string.app_name))
                    .build();
            startForeground(BodynodesConstants.SENSOR_SERVICE_NOTIFICATION_ID, notification);
        }

    }

    private void init() {
        new Handler().postDelayed(() -> {
            try {
                mMulticastConnector = new MulticastSocket(12346);
                mMulticastConnector.joinGroup(InetAddress.getByName("239.192.1.99"));
            } catch (UnknownHostException e ) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }

            if(mConnector == null) {
                try {
                    Log.d(TAG, "Creating the socket");
                    mConnector = new DatagramSocket(12345);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            IntentFilter intentFilter = new IntentFilter();
            intentFilter.addAction(BodynodesConstants.ACTION_SENSOR_GLOVE);
            LocalBroadcastManager.getInstance(this).registerReceiver(mSensorReceiver, intentFilter);

            run_connection_background();
            run_multicast_background();

            Thread thread = new Thread(() -> {
                if(mTimer!=null){
                    mTimer.cancel();
                    mTimer.purge();
                    mTimer = null;
                }
                mTimer = new Timer();
                mTimer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        Log.d(TAG, "Timer triggered");
                        if(checkAllOk()) {
                            sendSensorData();
                            checkAction();
                        }
                    }
                },5, AppData.getSensorIntervalMs(this));
            });
            thread.start();
        },5000);

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG,"Stopping service");
        AppData.setServiceRunning(false);
        if(mSensorManager!=null){
            mSensorManager.unregisterListener(this);
        }

        if(isReading()){
            stop();
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        if(sensorEvent.sensor.getType() == Sensor.TYPE_GAME_ROTATION_VECTOR ) {
            //Log.i(TAG, "onSensorChanged values = [ " + values[0] + ", " + values[1] + ", " + values[2] + ", " + values[3] + " ]");
            float[] values = sensorEvent.values;
            BodynodesUtils.realignQuat(values, mOrientationAbs);
        } else if(sensorEvent.sensor.getType() == ACCELETATION_TYPE) {
            //Log.i(TAG, "onSensorChanged values = [ " + values[0] + ", " + values[1] + ", " + values[2] + " ]");
            float[] values = sensorEvent.values;
            mAccelerationRel = new float[]{values[0], values[1], values[2]};
        }
    }

    private final BroadcastReceiver mSensorReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if(intent.getAction().equals(BodynodesConstants.ACTION_SENSOR_GLOVE)) {
                int[] gloveData = intent.getIntArrayExtra(BodynodesConstants.GLOVE_DATA);
                Log.d(TAG,"Glove change");
                JSONArray jsonArray = new JSONArray();
                JSONObject jsonObject = BodynodesProtocol.makeMessageWifi(
                        BodynodesData.getPlayerName(SensorServiceWifi.this),
                        BodynodesData.getGloveBodypart(SensorServiceWifi.this),
                        BodynodesConstants.SENSORTYPE_GLOVE_TAG,
                        gloveData);
                jsonArray.put(jsonObject);
                sendMessageWifiUdp(jsonArray.toString());
            }
        }
    };

    boolean isReading(){
        return mTimer!=null;
    }

    private void checkAction() {
        //Log.d(TAG, "mLastMessageReceived "+mLastMessageReceived.replace("\0","\\0"));

        int indexOpen = mLastDataReceived.indexOf("{");
        int indexClose = mLastDataReceived.indexOf("}");
        if(indexOpen < 0){
            //No starting point
            if(indexClose>=0){
                //But we got a end point
                //Let's trash partial jsons
                mLastDataReceived = mLastDataReceived.substring(indexClose+1);
            }
            return;
        }

        if(indexOpen > indexClose){
            //we got a starting point after the closing point
            //Let's trash partial jsons
            mLastDataReceived = mLastDataReceived.substring(indexOpen);
            indexClose = mLastDataReceived.indexOf("}");
        }
        if(indexClose<0){
            //JSON has not been closed, let's keep what we have and not read from it
            return;
        }
        String jsonText = mLastDataReceived.substring(indexOpen, indexClose+1);
        mLastDataReceived = mLastDataReceived.substring(indexClose+1);
        ////
        Log.d(TAG, "We received this json = "+jsonText);
        Intent intent = new Intent(BodynodesConstants.ACTION_RECEIVED);
        intent.putExtra(BodynodesConstants.KEY_JSON_ACTION,jsonText);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
        JSONObject actionJson;
        try {
            actionJson = new JSONObject(jsonText);
            if( BodynodesProtocol.parseActionWifi(actionJson) ==  BodynodesConstants.ACTION_CODE_HAPTIC) {
                vibrate(actionJson.getInt(BodynodesConstants.ACTION_HAPTIC_DURATIONMS_TAG),
                        actionJson.getInt(BodynodesConstants.ACTION_HAPTIC_STRENGTH_TAG));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        mLastDataReceived = "";
    }

    private void vibrate(int duration_ms, int strength) {
        Vibrator v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            if(strength < 1 ){
                strength = 1;
            }
            if(strength > 255){
                strength = 255;
            }
            v.vibrate(VibrationEffect.createOneShot(duration_ms, strength));
        } else {
            //deprecated in API 26
            v.vibrate(duration_ms);
        }
    }

    private void run_connection_background() {
        mDataConnectionThread = new Thread(() -> {
            if(AppData.isServiceRunning()) {
                try {
                    byte[] buffer = new byte[2048];
                    DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                    if (mConnector != null) {
                        mConnector.receive(packet);
                        mLastDataReceived = new String(buffer, 0, packet.getLength());
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });
        mDataConnectionThread.start();
    }

    private void run_multicast_background() {
        mMulticastConnectionThread = new Thread(() -> {
            if(AppData.isServiceRunning()) {
                try {
                    byte[] buffer = new byte[128];
                    DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                    if (mMulticastConnector != null) {
                        mMulticastConnector.receive(packet);
                        mLastMulticastReceived = new String(buffer, 0, packet.getLength());
                        mLastMulticastIpAddress = packet.getAddress().getHostAddress();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });
        mMulticastConnectionThread.start();
    }

    private boolean checkAllOk() {
        if(AppData.getCommunicationType(this) != BodynodesConstants.COMMUNICATION_TYPE_WIFI) {
            Log.d(TAG, "Wrong communication type");
            return false;
        }
        if(AppData.isCommunicationWaitingACK()){
            Log.d(TAG, "Waiting for ACK");
            if(checkForMulticastBN()){
                saveHostInfo();
            }
            if(hasHostInfo()) {
                sendACKN();
                if(checkForACKH()) {
                    AppData.setCommunicationConnected();
                    LocalBroadcastManager.getInstance(this).sendBroadcast(new Intent(BodynodesConstants.ACTION_UPDATE_UI));
                    Log.d(TAG, "Connected to Server with IP = "+mServerIpAddress);
                }
            }
            return false;
        } else if(AppData.isCommunicationDisconnected()){
            Log.d(TAG, "Disconnected");
            AppData.setCommunicationWaitingACK();
            LocalBroadcastManager.getInstance(this).sendBroadcast(new Intent(BodynodesConstants.ACTION_UPDATE_UI));
            return false;
        } else {
            Log.d(TAG, "Connected");
            return true;
        }
    }

    private boolean bigChangeValues(float[] values, float[] prev_values, int num_components, float big_diff) {
        boolean somethingChanged=false;

        for(int component=0; component<num_components; ++component){
            if(values[component]< prev_values[component]-big_diff ||
                    prev_values[component]+big_diff<values[component]) {
                somethingChanged = true;
                break;
            }
        }
        if(somethingChanged){
            System.arraycopy(values, 0, prev_values, 0, num_components);
        }
        return somethingChanged;
    }

    private void sendSensorData() {
        if(AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_WIFI){
            sendSensorDataViaWifiUdp();
        } else {
            Log.w(TAG,"Communication type not properly set");
        }

    }

    private void sendACKN() {
        try {
            String msg = "ACKN";
            int msg_length = msg.length();
            byte[] message = msg.getBytes();
            DatagramPacket packet = new DatagramPacket(message, msg_length, mServerIpAddress, 12345);
            mConnector.send(packet);
            Log.d(TAG,"Sending an ACKN");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    private boolean checkForACKH() {
        return mLastDataReceived.contains("ACKH");
    }

    private boolean checkForMulticastBN() {
        return mLastMulticastReceived.contains("BN");
    }

    private void saveHostInfo(){
        Log.d(TAG,"Saving Host");
        mServerIpAddress =  BodynodesUtils.getInetAddressFromStr(mLastMulticastIpAddress);
    }

    private boolean hasHostInfo() {
        return mServerIpAddress != null;
    }

    private void sendSensorDataViaWifiUdp() {
        //Log.d(TAG,"sendSensorDataViaWifi");
        boolean anydataToSend = false;
        JSONArray jsonArray = new JSONArray();
        if(BodynodesData.isOrientationAbsSensorEnabled(this) && bigChangeValues(mOrientationAbs, mPrevOrientationAbs, 4, BodynodesConstants.BIG_ORIENTATION_ABS_DIFF)){
            Log.d(TAG,"Orientation big change");
            JSONObject jsonObject = BodynodesProtocol.makeMessageWifi(
                    BodynodesData.getPlayerName(this),
                    BodynodesData.getBodypart(this),
                    BodynodesConstants.SENSORTYPE_ORIENTATION_ABS_TAG,
                    mOrientationAbs);
            jsonArray.put(jsonObject);
            anydataToSend = true;
        }
        if(BodynodesData.isAccelerationRelSensorEnabled(this) && bigChangeValues(mAccelerationRel, mPrevAccelerationRel, 3, BodynodesConstants.BIG_ACCELERATION_REL_DIFF)){
            Log.d(TAG,"Acceleration big change");
            JSONObject jsonObject = BodynodesProtocol.makeMessageWifi(
                    BodynodesData.getPlayerName(this),
                    BodynodesData.getBodypart(this),
                    BodynodesConstants.SENSORTYPE_ACCELERATION_REL_TAG,
                    mAccelerationRel);
            jsonArray.put(jsonObject);
            anydataToSend = true;
        }
        if(anydataToSend) {
            sendMessageWifiUdp(jsonArray.toString());
        }
    }

    private void stop() {
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                mTimer.cancel();
                mTimer.purge();
                mTimer = null;

                if(mConnector !=null){
                    mConnector.close();
                    mConnector = null;
                }
                if(mMulticastConnector !=null){
                    mMulticastConnector.close();
                    mMulticastConnector = null;
                }
                AppData.setCommunicationState(BodynodesConstants.COMMUNICATION_STATE_DISCONNECTED);
                LocalBroadcastManager.getInstance(SensorServiceWifi.this).sendBroadcast(new Intent(BodynodesConstants.ACTION_UPDATE_UI));
            }
        });
        thread.start();
    }

    private void sendMessageWifiUdp(String msg) {

        new Thread(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "sending = " + msg);
                int msg_length = msg.length();
                byte[] message = msg.getBytes();
                DatagramPacket packet = new DatagramPacket(message, msg_length, mServerIpAddress, 12345);
                try {
                    if(mConnector != null) {
                        mConnector.send(packet);
                    } else {
                        Log.e(TAG, "mConnector is null");
                    }
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        }).start();
    }
}
