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

package eu.bodynodesdev.sensor.services;

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

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;
import androidx.core.app.ServiceCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import android.util.Log;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.UnknownHostException;
import java.util.Timer;
import java.util.TimerTask;

import eu.bodynodesdev.common.BnConstants;
import eu.bodynodesdev.common.BnProtocol;

import eu.bodynodesdev.sensor.BnAppConstants;
import eu.bodynodesdev.sensor.BodynodesUtils;
import eu.bodynodesdev.sensor.data.AppData;
import eu.bodynodesdev.sensor.data.BnSensorAppData;
import eu.bodynodesdev.sensor.R;

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
    private long mLastSentTime = 0;
    private long mLastRecTime = 0;
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

        mLastSentTime = 0;
        mLastRecTime = 0;

        Log.i(TAG,"orientation_abs_enabled = "+ BnSensorAppData.isOrientationAbsSensorEnabled(this));
        Log.i(TAG,"acceleration_rel_enabled = "+ BnSensorAppData.isAccelerationRelSensorEnabled(this));
        if (mSensorManager != null) {
            if(BnSensorAppData.isOrientationAbsSensorEnabled(this)) {
                mOrientationAbsSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
                mSensorManager.registerListener(this, mOrientationAbsSensor, AppData.getSensorIntervalMs(this) * 1000);
            }
            if(BnSensorAppData.isAccelerationRelSensorEnabled(this)) {
                mAccelerationRelSensor = mSensorManager.getDefaultSensor(ACCELETATION_TYPE);
                mSensorManager.registerListener(this, mAccelerationRelSensor, AppData.getSensorIntervalMs(this) * 1000);
            }
        }
        if (AppData.getCommunicationType(this) == BnConstants.COMMUNICATION_TYPE_WIFI) {
            init();
        } else {
            Log.i(TAG,"Wrong communication type for " + TAG);
            stopSelf();
        }
    }

    @SuppressLint("ForegroundServiceType")
    private void createNotification() {
        String NOTIFICATION_CHANNEL_ID = BnAppConstants.NOTIFICATION_CHANNEL_ID;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            String channelName = BnAppConstants.NOTIFICATION_CHANNEL_NAME;
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
            ServiceCompat.startForeground(this, BnAppConstants.SENSOR_SERVICE_NOTIFICATION_ID, notification, ServiceInfo.FOREGROUND_SERVICE_TYPE_REMOTE_MESSAGING);
        } else {
            NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this, NOTIFICATION_CHANNEL_ID);
            Notification notification = notificationBuilder.setOngoing(true)
                    .setSmallIcon(R.drawable.bn_logo_topbar)
                    .setContentTitle(getText(R.string.app_name))
                    .build();
            startForeground(BnAppConstants.SENSOR_SERVICE_NOTIFICATION_ID, notification);
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
            intentFilter.addAction(BnAppConstants.ACTION_GLOVE_SENSOR_MESSAGE);
            intentFilter.addAction(BnAppConstants.ACTION_RESET_MESSAGE);
            LocalBroadcastManager.getInstance(this).registerReceiver(mMessagesToSendReceiver, intentFilter);

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
                        //og.d(TAG, "Timer triggered");
                        if(checkAllOk()) {
                            sendSensorData();
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

    private final BroadcastReceiver mMessagesToSendReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if(intent.getAction().equals(BnAppConstants.ACTION_GLOVE_SENSOR_MESSAGE)) {
                int[] gloveData = intent.getIntArrayExtra(BnAppConstants.GLOVE_SENSOR_DATA);
                Log.d(TAG,"Glove data to send");
                JSONArray jsonArray = new JSONArray();
                JSONObject jsonObject = BnProtocol.makeMessageJson(
                        BnSensorAppData.getPlayerName(SensorServiceWifi.this),
                        BnSensorAppData.getGloveBodypart(SensorServiceWifi.this),
                        BnConstants.SENSORTYPE_GLOVE_TAG,
                        gloveData);
                jsonArray.put(jsonObject);
                sendMessageWifiUdp(jsonArray.toString());
            } else if(intent.getAction().equals(BnAppConstants.ACTION_RESET_MESSAGE)){
                Log.d(TAG,"Reset message to send");
                JSONArray jsonArray = new JSONArray();
                JSONObject jsonObject1 = BnProtocol.makeMessageJson(
                        BnSensorAppData.getPlayerName(SensorServiceWifi.this),
                        BnSensorAppData.getBodypart(SensorServiceWifi.this),
                        BnConstants.SENSORTYPE_ORIENTATION_ABS_TAG,
                        BnConstants.MESSAGE_VALUE_RESET_TAG);
                JSONObject jsonObject2 = BnProtocol.makeMessageJson(
                        BnSensorAppData.getPlayerName(SensorServiceWifi.this),
                        BnSensorAppData.getBodypart(SensorServiceWifi.this),
                        BnConstants.SENSORTYPE_ACCELERATION_REL_TAG,
                        BnConstants.MESSAGE_VALUE_RESET_TAG);
                jsonArray.put(jsonObject1);
                jsonArray.put(jsonObject2);
                sendMessageWifiUdp(jsonArray.toString());
            }
        }
    };

    boolean isReading(){
        return mTimer!=null;
    }

    private void checkForActions() {
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
        Log.d(TAG, "We received this json = "+jsonText);
        Intent intent = new Intent(BnAppConstants.ACTION_RECEIVED);
        intent.putExtra(BnAppConstants.KEY_JSON_ACTION,jsonText);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
        mLastDataReceived = "";
    }

    private void run_connection_background() {
        mDataConnectionThread = new Thread(() -> {
            while(AppData.isServiceRunning()) {
                try {
                    try {
                        Thread.sleep(AppData.getSensorIntervalMs(this));
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                        return;
                    }
                    byte[] buffer = new byte[2048];
                    DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                    if (mConnector != null) {
                        mConnector.receive(packet);
                        mLastDataReceived = new String(buffer, 0, packet.getLength());
                    }
                } catch (Exception e) {
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
        if(AppData.getCommunicationType(this) != BnConstants.COMMUNICATION_TYPE_WIFI) {
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
                    LocalBroadcastManager.getInstance(this).sendBroadcast(new Intent(BnAppConstants.ACTION_UPDATE_UI));
                    Log.d(TAG, "Connected to Server with IP = "+mServerIpAddress);
                }
            }
            return false;
        } else if(AppData.isCommunicationDisconnected()){
            Log.d(TAG, "Disconnected");
            AppData.setCommunicationWaitingACK();
            LocalBroadcastManager.getInstance(this).sendBroadcast(new Intent(BnAppConstants.ACTION_UPDATE_UI));
            return false;
        } else {
            // Connected to wifi and server
            if(millis() - mLastSentTime > BnConstants.CONNECTION_KEEP_ALIVE_SEND_INTERVAL_MS){
                sendACKN();
            }
            if(millis() - mLastRecTime > BnConstants.CONNECTION_KEEP_ALIVE_REC_INTERVAL_MS){
                AppData.setCommunicationDisconnected();
            }
            if(!checkForACKH()) {
                checkForActions();
            }

            //Log.d(TAG, "Connected");
            return true;
        }
    }

    private long millis() {
        return System.currentTimeMillis();
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
        if(AppData.getCommunicationType(this) == BnConstants.COMMUNICATION_TYPE_WIFI){
            sendSensorDataViaWifiUdp();
        } else {
            Log.w(TAG,"Communication type not properly set");
        }

    }

    private void sendACKN() {
        if(mConnector == null){
            return;
        }
        if(millis() - mLastSentTime < BnConstants.CONNECTION_ACK_INTERVAL_MS){
            return;
        }
        try {
            String msg = "ACKN";
            int msg_length = msg.length();
            byte[] message = msg.getBytes();
            DatagramPacket packet = new DatagramPacket(message, msg_length, mServerIpAddress, 12345);
            mConnector.send(packet);
            mLastSentTime = millis();
            Log.d(TAG,"Sending an ACKN");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    private boolean checkForACKH() {
        if (mLastDataReceived.contains("ACKH") ){
            mLastRecTime = millis();
            mLastDataReceived = "";
            return true;
        }
        return false;
    }

    private boolean checkForMulticastBN() {
        if(mLastMulticastReceived.contains(BnSensorAppData.getMulticastGroup(this))){
            mLastRecTime = millis();
            mLastMulticastReceived = "";
            return true;
        }
        return false;
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
        if(BnSensorAppData.isOrientationAbsSensorEnabled(this) && bigChangeValues(mOrientationAbs, mPrevOrientationAbs, 4, BnConstants.BIG_ORIENTATION_ABS_DIFF)){
            Log.d(TAG,"Orientation big change");
            JSONObject jsonObject = BnProtocol.makeMessageJson(
                    BnSensorAppData.getPlayerName(this),
                    BnSensorAppData.getBodypart(this),
                    BnConstants.SENSORTYPE_ORIENTATION_ABS_TAG,
                    mOrientationAbs);
            jsonArray.put(jsonObject);
            anydataToSend = true;
        }
        if(BnSensorAppData.isAccelerationRelSensorEnabled(this) && bigChangeValues(mAccelerationRel, mPrevAccelerationRel, 3, BnConstants.BIG_ACCELERATION_REL_DIFF)){
            Log.d(TAG,"Acceleration big change");
            JSONObject jsonObject = BnProtocol.makeMessageJson(
                    BnSensorAppData.getPlayerName(this),
                    BnSensorAppData.getBodypart(this),
                    BnConstants.SENSORTYPE_ACCELERATION_REL_TAG,
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
                AppData.setCommunicationState(BnConstants.COMMUNICATION_STATE_DISCONNECTED);
                LocalBroadcastManager.getInstance(SensorServiceWifi.this).sendBroadcast(new Intent(BnAppConstants.ACTION_UPDATE_UI));
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
                        mLastSentTime = millis();
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
