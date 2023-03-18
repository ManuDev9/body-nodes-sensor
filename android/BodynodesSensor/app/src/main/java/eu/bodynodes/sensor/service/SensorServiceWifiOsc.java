/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Manuel Bottini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package eu.bodynodes.sensor.service;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
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
import androidx.core.app.NotificationCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.illposed.osc.OSCMessage;
import com.illposed.osc.OSCMessageEvent;
import com.illposed.osc.OSCMessageListener;
import com.illposed.osc.OSCSerializeException;
import com.illposed.osc.OSCSerializerAndParserBuilder;
import com.illposed.osc.argument.ArgumentHandler;
import com.illposed.osc.argument.handler.Activator;
import com.illposed.osc.transport.OSCPortIn;
import com.illposed.osc.transport.OSCPortOut;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import eu.bodynodes.sensor.BodynodesConstants;
import eu.bodynodes.sensor.BodynodesProtocol;
import eu.bodynodes.sensor.BodynodesUtils;
import eu.bodynodes.sensor.R;
import eu.bodynodes.sensor.data.AppData;
import eu.bodynodes.sensor.data.BodynodesData;

public class SensorServiceWifiOsc extends Service implements SensorEventListener, OSCMessageListener {

    private final static String TAG = "SensorServiceWifiOsc";

    private long mLastTime = 0;
    private String mLastMessageReceived = "";

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private SensorManager mSensorManager;
    private Sensor mOrientationAbsSensor;
    private Sensor mAccelerationRelSensor;

    private Timer mTimerReadSensor;
    private float[] mOrientationAbs;
    private float[] mAccelerationRel;
    private float[] mPrevOrientationAbs;
    private float[] mPrevAccelerationRel;

    private OSCPortOut mOscPortOut;
    private OSCPortIn mOscPortIn;

    @Override
    public void onCreate() {
        super.onCreate();
        createNotification();
        mOrientationAbs = new float[]{0, 0, 0, 0};
        mAccelerationRel = new float[]{0, 0, 0, 0};
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
                mAccelerationRelSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
                mSensorManager.registerListener(this, mAccelerationRelSensor, AppData.getSensorIntervalMs(this) * 1000);
            }
        }
        if (AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_WIFI_OSC) {
            startCommunicationWifi();
        } else {
            Log.i(TAG,"Wrong communication type for " + TAG);
            stopSelf();
        }
    }

    private void createNotification() {
        String NOTIFICATION_CHANNEL_ID = "eu.bodynodes.sensor";
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            String channelName = "Bodynodes Service";
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
            startForeground(BodynodesConstants.SENSOR_SERVICE_NOTIFICATION_ID, notification);
        } else {
            NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this, NOTIFICATION_CHANNEL_ID);
            Notification notification = notificationBuilder.setOngoing(true)
                    .setSmallIcon(R.drawable.bn_logo_topbar)
                    .setContentTitle(getText(R.string.app_name))
                    .build();
            startForeground(BodynodesConstants.SENSOR_SERVICE_NOTIFICATION_ID, notification);
        }
    }

    private void startCommunicationWifi() {
        new Handler().postDelayed(() -> {
            createOscPorts();
            startReadingSensor();
            startListening();
        },5000);
    }

    // messages received from mOscPortIn
    @Override
    public void acceptMessage(OSCMessageEvent event) {
        Log.i(TAG, "Message received: " + event.getMessage().toString() + " from " +event.getMessage().getAddress());
        mLastMessageReceived = event.getMessage().toString();
    }

    private void startListening() {
        try {
            mOscPortIn = new OSCPortIn(AppData.getLocalPortInNumber(this));
            Thread thread = new Thread(() -> {
                try  {
                    mOscPortIn.connect();
                    Log.i(TAG, "mOscPortIn.isConnected() = " +mOscPortIn.isConnected());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            });
            thread.start();
            AppData.setCommunicationState(BodynodesConstants.COMMUNICATION_STATE_WAITING_ACK);
            updateUI();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void createOscPorts() {
        try {
            //Log.i(TAG, "Osc output ip address is = "+AppData.getRemoteIPAddress(this));

            OSCSerializerAndParserBuilder serializer = new OSCSerializerAndParserBuilder();
            serializer.setUsingDefaultHandlers(false);
            List<ArgumentHandler> defaultParserTypes = Activator.createSerializerTypes();
            defaultParserTypes.remove(16);
            char typeChar = 'a';
            for (ArgumentHandler argumentHandler:defaultParserTypes) {
                serializer.registerArgumentHandler(argumentHandler, typeChar);
                typeChar++;
            }
            //mOscPortOut = new OSCPortOut(serializer,
            //        new InetSocketAddress(AppData.getRemoteIPAddress(this), AppData.getRemotePortOutNumber(this)));

            Thread thread = new Thread(() -> {
                try  {
                    mOscPortOut.connect();
                    Log.i(TAG, "mOscPortOut.isConnected() = " +mOscPortOut.isConnected());
                } catch (Exception e) {
                    e.printStackTrace();
                }
            });
            thread.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG,"Stopping service");
        AppData.setServiceRunning(false);
        if(mSensorManager!=null){
            mSensorManager.unregisterListener(this);
        }
        if(isReadingSensor()){
            stopReading();
        }
        try {
            if(mOscPortOut != null){
                mOscPortOut.disconnect();
                mOscPortOut.close();
                mOscPortOut = null;
            }
            if(mOscPortIn != null) {
                mOscPortIn.disconnect();
                mOscPortIn.close();
                mOscPortIn = null;
            }
            AppData.setCommunicationState(BodynodesConstants.COMMUNICATION_STATE_DISCONNECTED);
            updateUI();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        if (sensorEvent.sensor.getType() == Sensor.TYPE_GAME_ROTATION_VECTOR ||
                sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            float[] values = sensorEvent.values;
            if(sensorEvent.sensor.getType() == Sensor.TYPE_GAME_ROTATION_VECTOR ) {
                //Log.i(TAG, "onSensorChanged values = [ " + values[0] + ", " + values[1] + ", " + values[2] + ", " + values[3] + " ]");
                BodynodesUtils.realignQuat(values, mOrientationAbs);
            } else if(sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                //Log.i(TAG, "onSensorChanged values = [ " + values[0] + ", " + values[1] + ", " + values[2] + " ]");
                mAccelerationRel = new float[]{values[0], values[1], values[2]};
            }
            if(!isReadingSensor()) {
                startReadingSensor();
            }
        }
    }

    boolean isReadingSensor(){
        return mTimerReadSensor !=null;
    }

    private void startReadingSensor() {
        Thread thread = new Thread(() -> {
            if(mTimerReadSensor!=null){
                mTimerReadSensor.cancel();
                mTimerReadSensor.purge();
                mTimerReadSensor = null;
            }
            mTimerReadSensor = new Timer();
            mTimerReadSensor.schedule(new TimerTask() {
                @Override
                public void run() {
                    if(checkConnection()) {
                        sendSensorData();
                        checkAction();
                    }
                }
            },5,AppData.getSensorIntervalMs(this));
        });
        thread.start();
    }

    private void checkAction() {
        if(mLastMessageReceived.isEmpty()){
            return;
        }
        Log.d(TAG, "We received this json = "+mLastMessageReceived);
        Intent intent = new Intent(BodynodesConstants.ACTION_RECEIVED);
        intent.putExtra(BodynodesConstants.KEY_JSON_ACTION, mLastMessageReceived);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
        JSONObject actionJson;
        try {
            actionJson = new JSONObject(mLastMessageReceived);
            if( BodynodesProtocol.parseActionWifi(actionJson) ==  BodynodesConstants.ACTION_CODE_HAPTIC) {
                vibrate(actionJson.getInt(BodynodesConstants.ACTION_HAPTIC_DURATIONMS_TAG),
                        actionJson.getInt(BodynodesConstants.ACTION_HAPTIC_STRENGTH_TAG));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        } finally {
            mLastMessageReceived = "";
        }

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

    private boolean checkConnection() {
        if(mOscPortOut ==null){
            return false;
        }
        if(AppData.isCommunicationConnected()){
            return true;
        }
        if(System.currentTimeMillis()-mLastTime < 1000){
            return false;
        }
        mLastTime=System.currentTimeMillis();
        if(mOscPortOut.isConnected()){
            AppData.setCommunicationConnected();
            updateUI();
            return true;
        } else {
            return false;
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
        if(AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_WIFI_OSC){
            sendSensorDataViaWifiOsc();
        } else {
            Log.w(TAG,"Communication type not properly set");
        }
    }

    private void sendSensorDataViaWifiOsc() {
        try {
            if(BodynodesData.isOrientationAbsSensorEnabled(this) && bigChangeValues(mOrientationAbs, mPrevOrientationAbs, 4, BodynodesConstants.BIG_ORIENTATION_ABS_DIFF)){
                Log.d(TAG,"Orientation big change");
                OSCMessage message = BodynodesProtocol.makeMessageWifiOsc(
                        BodynodesData.getPlayerName(this),
                        BodynodesData.getBodypart(this),
                        BodynodesConstants.SENSORTYPE_ORIENTATION_ABS_TAG,
                        mOrientationAbs);
                mOscPortOut.send(message);
            }
            if(BodynodesData.isAccelerationRelSensorEnabled(this) && bigChangeValues(mAccelerationRel, mPrevAccelerationRel, 3, BodynodesConstants.BIG_ACCELERATION_REL_DIFF)){
                Log.d(TAG,"Acceleration big change");
                OSCMessage message = BodynodesProtocol.makeMessageWifiOsc(
                        BodynodesData.getPlayerName(this),
                        BodynodesData.getBodypart(this),
                        BodynodesConstants.SENSORTYPE_ACCELERATION_REL_TAG,
                        mAccelerationRel);
                mOscPortOut.send(message);
            }
        } catch (java.lang.ArrayIndexOutOfBoundsException e) {
            e.printStackTrace();
        } catch (java.nio.BufferOverflowException e) {
            e.printStackTrace();
        } catch (java.lang.IllegalArgumentException e) {
            e.printStackTrace();
        } catch (OSCSerializeException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void updateUI(){
        LocalBroadcastManager.getInstance(SensorServiceWifiOsc.this).sendBroadcast(new Intent(BodynodesConstants.ACTION_UPDATE_UI));
    }

    private void stopReading() {
        Thread thread = new Thread(() -> {
            mTimerReadSensor.cancel();
            mTimerReadSensor.purge();
            mTimerReadSensor = null;
            AppData.setCommunicationState(BodynodesConstants.COMMUNICATION_STATE_DISCONNECTED);
            updateUI();
        });
        thread.start();
    }

}
