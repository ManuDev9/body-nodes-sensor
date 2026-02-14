/*
  MIT License

  Copyright (c) 2024-2025 Manuel Bottini

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

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.AdvertiseSettings;
import android.bluetooth.le.BluetoothLeAdvertiser;
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
import android.os.Binder;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.ParcelUuid;
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
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

import eu.bodynodesdev.common.BnConstants;
import eu.bodynodesdev.sensor.BnAppConstants;
import eu.bodynodesdev.sensor.BodynodesUtils;
import eu.bodynodesdev.sensor.R;
import eu.bodynodesdev.sensor.data.AppData;
import eu.bodynodesdev.sensor.data.BnSensorAppData;

public class SensorServiceBLE extends Service implements SensorEventListener {

    private final static String TAG = "SensorServiceBLE";

    private BluetoothAdapter mBluetoothAdapter = null;

    private BluetoothManager mBluetoothManager = null;

    private BluetoothGattServer mGattServer = null;

    private BluetoothGattCharacteristic mOrientationAbsChara = null;
    private BluetoothGattCharacteristic mAccelerationRelChara = null;
    private BluetoothGattCharacteristic mGloveChara = null;
    private BluetoothGattCharacteristic mShoeChara = null;
    private BluetoothLeAdvertiser mAdvertiser = null;
    private AdvertiseCallback mAdvertiseCallback = null;
    private BluetoothDevice mDevice = null;

    private String mLastDataReceived = "";

    public class LocalBinder extends Binder {
        SensorServiceBLE getService() {
            return SensorServiceBLE.this;
        }
    }
    private final IBinder mBinder = new LocalBinder();

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder; // for AllServicesTest
    }

    private SensorManager mSensorManager;
    private Sensor mOrientationAbsSensor;
    private Sensor mAccelerationRelSensor;
    public static final int ACCELETATION_TYPE = Sensor.TYPE_LINEAR_ACCELERATION;
    private Timer mTimer;
    private boolean mIsRunning = false;
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

        Log.i(TAG, "orientation_abs_enabled = " + BnSensorAppData.isOrientationAbsSensorEnabled(this));
        Log.i(TAG, "acceleration_rel_enabled = " + BnSensorAppData.isAccelerationRelSensorEnabled(this));
        if (mSensorManager != null) {
            if (BnSensorAppData.isOrientationAbsSensorEnabled(this)) {
                mOrientationAbsSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
                mSensorManager.registerListener(this, mOrientationAbsSensor, AppData.getSensorIntervalMs(this) * 1000);
            }
            if (BnSensorAppData.isAccelerationRelSensorEnabled(this)) {
                mAccelerationRelSensor = mSensorManager.getDefaultSensor(ACCELETATION_TYPE);
                mSensorManager.registerListener(this, mAccelerationRelSensor, AppData.getSensorIntervalMs(this) * 1000);
            }
        }
        if (AppData.getCommunicationType(this) == BnAppConstants.COMMUNICATION_TYPE_BLE) {
            mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            mBluetoothAdapter = mBluetoothManager.getAdapter();
            mIsRunning = true;
            init();
        } else {
            throw new RuntimeException( "Wrong communication type for " + TAG);
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

            AppData.setCommunicationDisconnected();
            LocalBroadcastManager.getInstance(this).sendBroadcast(new Intent(BnAppConstants.ACTION_UPDATE_UI));

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED ||
                        ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_ADVERTISE) != PackageManager.PERMISSION_GRANTED) {
                    return;
                }
            }
            else {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                    return;
                }
            }

            if (!mBluetoothAdapter.isEnabled()) {
                Log.e(TAG, "Bluetooth is not enabled.");
                return;
            }
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                if (!mBluetoothAdapter.isMultipleAdvertisementSupported()) {
                    Log.e(TAG, "This device cannot act as a BLE Peripheral");
                    return;
                }
            }

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                mAdvertiser = mBluetoothAdapter.getBluetoothLeAdvertiser();
            }

            if (mAdvertiser == null) {
                Log.e(TAG, "Peripheral mode not supported on this device.");
                return;
            }

            mGattServer = mBluetoothManager.openGattServer(this, mGattServerCallback);
            if (mGattServer == null) {
                Log.e(TAG, "Unable to create GATT server");
                return;
            }

            BluetoothGattService bodynodesService = new BluetoothGattService(
                    UUID.fromString(BnConstants.BLE_SERVICE_UUID),
                    BluetoothGattService.SERVICE_TYPE_PRIMARY
            );

            final UUID CCCD_UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
            BluetoothGattCharacteristic playerChara = new BluetoothGattCharacteristic(
                    UUID.fromString(BnConstants.BLE_CHARA_PLAYER_UUID),
                    BluetoothGattCharacteristic.PROPERTY_READ ,
                    BluetoothGattCharacteristic.PERMISSION_READ
            );
            final BluetoothGattDescriptor playerCharaDescriptor = new BluetoothGattDescriptor(CCCD_UUID,
                    BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
            playerChara.addDescriptor(playerCharaDescriptor);
            playerChara.setValue(BnSensorAppData.getPlayerName(this));

            BluetoothGattCharacteristic bodypartChara = new BluetoothGattCharacteristic(
                    UUID.fromString(BnConstants.BLE_CHARA_BODYPART_UUID),
                    BluetoothGattCharacteristic.PROPERTY_READ ,
                    BluetoothGattCharacteristic.PERMISSION_READ
            );
            final BluetoothGattDescriptor bodypartCharaDescriptor = new BluetoothGattDescriptor(CCCD_UUID,
                    BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
            bodypartChara.addDescriptor(bodypartCharaDescriptor);
            bodypartChara.setValue(BnSensorAppData.getBodypart(this));

            mOrientationAbsChara = new BluetoothGattCharacteristic(
                    UUID.fromString(BnConstants.BLE_CHARA_ORIENTATION_ABS_VALUE_UUID),
                    BluetoothGattCharacteristic.PROPERTY_NOTIFY,
                    0
            );
            final BluetoothGattDescriptor orientationAbsCharaDescriptor = new BluetoothGattDescriptor(CCCD_UUID,
                    BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
            mOrientationAbsChara.addDescriptor(orientationAbsCharaDescriptor);

            mAccelerationRelChara = new BluetoothGattCharacteristic(
                    UUID.fromString(BnConstants.BLE_CHARA_ACCELERATION_REL_VALUE_UUID),
                    BluetoothGattCharacteristic.PROPERTY_NOTIFY,
                    0
            );
            final BluetoothGattDescriptor accelerationRelCharaDescriptor = new BluetoothGattDescriptor(CCCD_UUID,
                    BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
            mAccelerationRelChara.addDescriptor(accelerationRelCharaDescriptor);

            mGloveChara = new BluetoothGattCharacteristic(
                    UUID.fromString(BnConstants.BLE_CHARA_GLOVE_VALUE_UUID),
                    BluetoothGattCharacteristic.PROPERTY_NOTIFY,
                    0
            );
            final BluetoothGattDescriptor gloveCharaDescriptor = new BluetoothGattDescriptor(CCCD_UUID,
                    BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
            mGloveChara.addDescriptor(gloveCharaDescriptor);

            mShoeChara = new BluetoothGattCharacteristic(
                    UUID.fromString(BnConstants.BLE_CHARA_SHOE_UUID),
                    BluetoothGattCharacteristic.PROPERTY_NOTIFY,
                    0
            );
            final BluetoothGattDescriptor shoeCharaDescriptor = new BluetoothGattDescriptor(CCCD_UUID,
                    BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
            mShoeChara.addDescriptor(shoeCharaDescriptor);

            bodynodesService.addCharacteristic(playerChara);
            bodynodesService.addCharacteristic(bodypartChara);
            bodynodesService.addCharacteristic(mOrientationAbsChara);
            bodynodesService.addCharacteristic(mAccelerationRelChara);
            bodynodesService.addCharacteristic(mGloveChara);
            bodynodesService.addCharacteristic(mShoeChara);

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                AdvertiseSettings settings = new AdvertiseSettings.Builder()
                        .setAdvertiseMode(AdvertiseSettings.ADVERTISE_MODE_LOW_LATENCY)
                        .setConnectable(true)
                        .setTimeout(0)
                        .setTxPowerLevel(AdvertiseSettings.ADVERTISE_TX_POWER_HIGH)
                        .build();

                AdvertiseData data = new AdvertiseData.Builder()
                        .setIncludeDeviceName(true)
                        .addServiceUuid(new ParcelUuid(UUID.fromString(BnConstants.BLE_SERVICE_UUID)))
                        .build();

                mAdvertiseCallback = new AdvertiseCallback() {
                    @Override
                    public void onStartSuccess(AdvertiseSettings settingsInEffect) {
                        Log.d(TAG, "LE Advertise Started Successfully");
                    }

                    @Override
                    public void onStartFailure(int errorCode) {
                        Log.e(TAG, "LE Advertise Failed: " + errorCode);
                    }
                };
                mAdvertiser.startAdvertising(settings, data, mAdvertiseCallback);
            }

            mGattServer.addService(bodynodesService);

            IntentFilter intentFilter = new IntentFilter();
            intentFilter.addAction(BnAppConstants.ACTION_GLOVE_SENSOR_MESSAGE);
            LocalBroadcastManager.getInstance(this).registerReceiver(mMessagesToSendReceiver, intentFilter);

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

        if (mIsRunning) {
            mIsRunning = false;
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

    private final BroadcastReceiver mMessagesToSendReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if(BnAppConstants.ACTION_GLOVE_SENSOR_MESSAGE.equals(intent.getAction())) {
                int[] gloveData = intent.getIntArrayExtra(BnAppConstants.GLOVE_SENSOR_DATA);
                Log.d(TAG,"Glove data to send");
                if( gloveData == null ){
                    Log.e(TAG,"No data!");
                }
                assert gloveData != null;
                byte[] data = new byte[gloveData.length];
                // int[] intArray = new int[]{0, 0, 0, 0, 0, 0, 0, 0, 0};
                for( int count = 0; count < gloveData.length; count++ ) {
                    data[count] = (byte) gloveData[count];
                }
                sendNotification(mGloveChara, data);
            }
        }
    };

    boolean isReading() {
        return mTimer != null;
    }

    private void checkForActions() {
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
        Log.d(TAG, "We received this json = "+jsonText);
        Intent intent = new Intent(BnAppConstants.ACTION_RECEIVED);
        intent.putExtra(BnAppConstants.KEY_JSON_ACTION,jsonText);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
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


    private final BluetoothGattServerCallback mGattServerCallback = new BluetoothGattServerCallback() {

        @Override
        public void onConnectionStateChange(BluetoothDevice device, int status, int newState) {
            super.onConnectionStateChange(device, status, newState);
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.d(TAG,"Device connected!");
                mDevice = device;
                AppData.setCommunicationConnected();
                LocalBroadcastManager.getInstance(SensorServiceBLE.this).sendBroadcast(new Intent(BnAppConstants.ACTION_UPDATE_UI));
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.d(TAG,"Device disconnected!");
                mDevice = null;
                AppData.setCommunicationDisconnected();
                LocalBroadcastManager.getInstance(SensorServiceBLE.this).sendBroadcast(new Intent(BnAppConstants.ACTION_UPDATE_UI));
            }
        }

        @Override
        public void onCharacteristicReadRequest(BluetoothDevice device,
                                                int requestId,
                                                int offset,
                                                BluetoothGattCharacteristic characteristic) {

            if (ActivityCompat.checkSelfPermission(SensorServiceBLE.this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                mGattServer.sendResponse(
                        device,
                        requestId,
                        BluetoothGatt.GATT_SUCCESS,
                        offset,
                        characteristic.getValue()
                );
            }
        }

        @Override
        public void onDescriptorWriteRequest(BluetoothDevice device, int requestId,
                                             BluetoothGattDescriptor descriptor,
                                             boolean preparedWrite, boolean responseNeeded,
                                             int offset, byte[] value) {

            // Logic: If the client writes 0x01, they are subscribing.
            if (responseNeeded) {
                if (ActivityCompat.checkSelfPermission(SensorServiceBLE.this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                    mGattServer.sendResponse(device, requestId, BluetoothGatt.GATT_SUCCESS, 0, null);
                }
            }
        }
    };

    private boolean checkAllOk() {
        if(AppData.isCommunicationWaitingACK()){
            //Log.d(TAG, "Waiting for ACK");
            return false;
        } else if(AppData.isCommunicationDisconnected()){
            Log.d(TAG, "Disconnected");
            AppData.setCommunicationWaitingACK();
            LocalBroadcastManager.getInstance(this).sendBroadcast(new Intent(BnAppConstants.ACTION_UPDATE_UI));
            return false;
        } else {
            //Log.d(TAG, "Connected");
            LocalBroadcastManager.getInstance(this).sendBroadcast(new Intent(BnAppConstants.ACTION_UPDATE_UI));
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
        if(AppData.getCommunicationType(this) == BnAppConstants.COMMUNICATION_TYPE_BLE){
            sendSensorDataVia();
        } else {
            Log.e(TAG,"Communication type not properly set");
        }
    }

    private void sendSensorDataVia() {
        //Log.d(TAG,"sendSensorDataViaWifi");
        if(BnSensorAppData.isOrientationAbsSensorEnabled(this) && bigChangeValues(mOrientationAbs, mPrevOrientationAbs, 4,
                BnAppConstants.BIG_ORIENTATION_ABS_DIFF)){
            Log.d(TAG,"Orientation big change");
            byte[] data = new byte[16];
            ByteBuffer buffer = ByteBuffer.wrap(data);
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            for( int count = 0; count < mOrientationAbs.length; count++ ) {
                buffer.putFloat(mOrientationAbs[count]);
            }
            sendNotification(mOrientationAbsChara, data);
        }
        if(BnSensorAppData.isAccelerationRelSensorEnabled(this) && bigChangeValues(mAccelerationRel, mPrevAccelerationRel, 3,
                BnAppConstants.BIG_ORIENTATION_ABS_DIFF)){
            Log.d(TAG,"Acceleration big change");
            byte[] data = new byte[12];
            ByteBuffer buffer = ByteBuffer.wrap(data);
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            for( int count = 0; count < mAccelerationRel.length; count++ ) {
                buffer.putFloat(mAccelerationRel[count]);
            }
            sendNotification(mAccelerationRelChara, data);
        }
    }

    private void sendNotification(BluetoothGattCharacteristic chara, byte[] data) {
        chara.setValue(data);
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
            //Log.d(TAG,"Sending notification");
            mGattServer.notifyCharacteristicChanged(mDevice, chara, false);
        } else {
            Log.d(TAG,"No permission to send notification");
        }
    }

    private void stop() {
        Thread thread = new Thread(() -> {
            Log.i(TAG,"Stopping the BLE service");
            if(mTimer!=null) {
                mTimer.cancel();
                mTimer.purge();
                mTimer = null;
            }
            if (mAdvertiser != null ) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    if (ActivityCompat.checkSelfPermission(SensorServiceBLE.this, Manifest.permission.BLUETOOTH_ADVERTISE) == PackageManager.PERMISSION_GRANTED) {
                        mAdvertiser.stopAdvertising(mAdvertiseCallback);
                    }
                }
            }

            if (mGattServer != null) {
                mGattServer.cancelConnection(mDevice);
                mGattServer.clearServices();
                mGattServer.close();
                mGattServer = null;
            }
            mDevice = null;

            AppData.setCommunicationState(BnAppConstants.COMMUNICATION_STATE_DISCONNECTED);
            LocalBroadcastManager.getInstance(SensorServiceBLE.this).sendBroadcast(new Intent(BnAppConstants.ACTION_UPDATE_UI));
        });
        thread.start();
    }

}
