/*
  MIT License

  Copyright (c) 2024 Manuel Bottini

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

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.pm.ServiceInfo;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationCompat;
import androidx.core.app.ServiceCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

import eu.bodynodes.sensor.BodynodesConstants;
import eu.bodynodes.sensor.BodynodesProtocol;
import eu.bodynodes.sensor.BodynodesUtils;
import eu.bodynodes.sensor.R;
import eu.bodynodes.sensor.data.AppData;
import eu.bodynodes.sensor.data.BodynodesData;

public class SensorServiceBluetooth extends Service implements SensorEventListener {

    private final static String TAG = "SensorServiceBluetooth";

    private static final String SERVER_MAC_ADDRESS = "10:6F:D9:3D:82:50";
    private static final UUID APP_BT_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); // SPP UUID

    private static final int RFCOMM_PORT = 1; // Replace with the server's RFCOMM port

    private Thread mDataConnectionThread = null;
    private Thread mAcceptConnectionThread = null;
    private BluetoothAdapter mBluetoothAdapter = null;

    private BluetoothSocket mConnector = null;
    private BluetoothServerSocket mConnectorServer = null;

    private String mLastDataReceived = "";

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

        Log.i(TAG, "orientation_abs_enabled = " + BodynodesData.isOrientationAbsSensorEnabled(this));
        Log.i(TAG, "acceleration_rel_enabled = " + BodynodesData.isAccelerationRelSensorEnabled(this));
        if (mSensorManager != null) {
            if (BodynodesData.isOrientationAbsSensorEnabled(this)) {
                mOrientationAbsSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
                mSensorManager.registerListener(this, mOrientationAbsSensor, AppData.getSensorIntervalMs(this) * 1000);
            }
            if (BodynodesData.isAccelerationRelSensorEnabled(this)) {
                mAccelerationRelSensor = mSensorManager.getDefaultSensor(ACCELETATION_TYPE);
                mSensorManager.registerListener(this, mAccelerationRelSensor, AppData.getSensorIntervalMs(this) * 1000);
            }
        }
        if (AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_BLUETOOTH) {
            mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            init();
        } else {
            Log.i(TAG, "Wrong communication type for " + TAG);
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

            if (!mBluetoothAdapter.isEnabled()) {
                Log.e(TAG, "Bluetooth is not enabled.");
                return;
            }

            mConnectorServer = null;
            try {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    // Done in the Main Activity
                    return;
                }
                mConnectorServer = mBluetoothAdapter.listenUsingRfcommWithServiceRecord("BodynodesApp", APP_BT_UUID);
            } catch (IOException e) {
                Log.e(TAG, "Socket's listen() method failed", e);
            }

            IntentFilter intentFilter = new IntentFilter();
            intentFilter.addAction(BodynodesConstants.ACTION_SENSOR_GLOVE);
            LocalBroadcastManager.getInstance(this).registerReceiver(mSensorReceiver, intentFilter);

            run_connection_background();
            run_accept_background();

            Thread thread = new Thread(() -> {
                if (mTimer != null) {
                    mTimer.cancel();
                    mTimer.purge();
                    mTimer = null;
                }
                mTimer = new Timer();
                mTimer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        //Log.d(TAG, "Timer triggered");
                        if (checkAllOk()) {
                            sendSensorData();
                            checkAction();
                        }
                    }
                }, 5, AppData.getSensorIntervalMs(this));
            });
            thread.start();
        }, 5000);

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "Stopping service");
        AppData.setServiceRunning(false);
        if (mSensorManager != null) {
            mSensorManager.unregisterListener(this);
        }

        if (isReading()) {
            stop();
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        if (sensorEvent.sensor.getType() == Sensor.TYPE_GAME_ROTATION_VECTOR) {
            //Log.i(TAG, "onSensorChanged values = [ " + values[0] + ", " + values[1] + ", " + values[2] + ", " + values[3] + " ]");
            float[] values = sensorEvent.values;
            BodynodesUtils.realignQuat(values, mOrientationAbs);
        } else if (sensorEvent.sensor.getType() == ACCELETATION_TYPE) {
            //Log.i(TAG, "onSensorChanged values = [ " + values[0] + ", " + values[1] + ", " + values[2] + " ]");
            float[] values = sensorEvent.values;
            mAccelerationRel = new float[]{values[0], values[1], values[2]};
        }
    }

    private final BroadcastReceiver mSensorReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(BodynodesConstants.ACTION_SENSOR_GLOVE)) {
                int[] gloveData = intent.getIntArrayExtra(BodynodesConstants.GLOVE_DATA);
                Log.d(TAG, "Glove change");
                JSONArray jsonArray = new JSONArray();
                JSONObject jsonObject = BodynodesProtocol.makeMessageWifi(
                        BodynodesData.getPlayerName(SensorServiceBluetooth.this),
                        BodynodesData.getGloveBodypart(SensorServiceBluetooth.this),
                        BodynodesConstants.SENSORTYPE_GLOVE_TAG,
                        gloveData);
                jsonArray.put(jsonObject);
                sendMessageWifiUdp(jsonArray.toString());
            }
        }
    };

    boolean isReading() {
        return mTimer != null;
    }

    private void checkAction() {
        //Log.d(TAG, "mLastMessageReceived "+mLastMessageReceived.replace("\0","\\0"));

        int indexOpen = mLastDataReceived.indexOf("{");
        int indexClose = mLastDataReceived.indexOf("}");
        if (indexOpen < 0) {
            //No starting point
            if (indexClose >= 0) {
                //But we got a end point
                //Let's trash partial jsons
                mLastDataReceived = mLastDataReceived.substring(indexClose + 1);
            }
            return;
        }

        if (indexOpen > indexClose) {
            //we got a starting point after the closing point
            //Let's trash partial jsons
            mLastDataReceived = mLastDataReceived.substring(indexOpen);
            indexClose = mLastDataReceived.indexOf("}");
        }
        if (indexClose < 0) {
            //JSON has not been closed, let's keep what we have and not read from it
            return;
        }
        String jsonText = mLastDataReceived.substring(indexOpen, indexClose + 1);
        mLastDataReceived = mLastDataReceived.substring(indexClose + 1);
        ////
        Log.d(TAG, "We received this json = " + jsonText);
        Intent intent = new Intent(BodynodesConstants.ACTION_RECEIVED);
        intent.putExtra(BodynodesConstants.KEY_JSON_ACTION, jsonText);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
        JSONObject actionJson;
        try {
            actionJson = new JSONObject(jsonText);
            if (BodynodesProtocol.parseActionWifi(actionJson) == BodynodesConstants.ACTION_CODE_HAPTIC) {
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
            if (strength < 1) {
                strength = 1;
            }
            if (strength > 255) {
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
            while (AppData.isServiceRunning()) {
                try {
                    Thread.sleep(AppData.getSensorIntervalMs(this));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                    return;
                }
                try {
                    byte[] buffer = new byte[2048];
                    int bytes = mConnector.getInputStream().read(buffer);
                    mLastDataReceived = new String(buffer, 0, bytes);
                } catch (NullPointerException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    Log.w(TAG, "The bluetooth connection terminated");
                    mConnector = null;
                    e.printStackTrace();
                }
            }
        });
        mDataConnectionThread.start();
    }

    private void run_accept_background() {

        mAcceptConnectionThread = new Thread(() -> {
            while (AppData.isServiceRunning()) {
                try {
                    Thread.sleep(AppData.getSensorIntervalMs(this));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                Log.d(TAG, "Accepting connections");
                if (mConnector != null) {
                    return;
                }
                try {
                    mConnector = mConnectorServer.accept();
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
                if(mConnector != null){
                    Log.w(TAG, "We got a connection!");
                }
            }
        });
        mAcceptConnectionThread.start();
    }

    private boolean checkAllOk() {
        if(AppData.getCommunicationType(this) != BodynodesConstants.COMMUNICATION_TYPE_BLUETOOTH) {
            Log.d(TAG, "Wrong communication type");
            return false;
        }
        if(AppData.isCommunicationWaitingACK()){
            //Log.d(TAG, "Waiting for ACK");
            sendACKN();
            if(checkForACKH()) {
                AppData.setCommunicationConnected();
                LocalBroadcastManager.getInstance(this).sendBroadcast(new Intent(BodynodesConstants.ACTION_UPDATE_UI));
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
        if(AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_BLUETOOTH){
            sendSensorDataViaBluetooth();
        } else {
            Log.w(TAG,"Communication type not properly set");
        }

    }

    private void sendACKN() {
        try {
            if(mConnector == null){
                return;
            }
            String msg = "ACKN";
            mConnector.getOutputStream().write(msg.getBytes());
            mConnector.getOutputStream().flush();
            Log.d(TAG, "Sending an ACKN");
        } catch (NullPointerException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    private boolean checkForACKH() {
        return mLastDataReceived.contains("ACKH");
    }

    private void sendSensorDataViaBluetooth() {
        //Log.d(TAG,"sendSensorDataViaBluetooth");
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
                    try {
                        mConnector.close();
                        mConnectorServer.close();
                    } catch (IOException e) {
                        throw new RuntimeException(e);
                    }
                    mConnector = null;
                    mConnectorServer = null;
                }

                AppData.setCommunicationState(BodynodesConstants.COMMUNICATION_STATE_DISCONNECTED);
                LocalBroadcastManager.getInstance(SensorServiceBluetooth.this).sendBroadcast(new Intent(BodynodesConstants.ACTION_UPDATE_UI));
            }
        });
        thread.start();
        mDataConnectionThread.interrupt();
        mAcceptConnectionThread.interrupt();
        mDataConnectionThread = null;
        mAcceptConnectionThread = null;
    }

    private void sendMessageWifiUdp(String msg) {

        new Thread(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "sending = " + msg);
                try {
                    if(mConnector != null) {
                        mConnector.getOutputStream().write(msg.getBytes());
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
