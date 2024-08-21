/*
 * MIT License
 *
 * Copyright (c) 2019-2024 Manuel Bottini
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

package eu.bodynodes.sensor.pages;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Handler;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

import eu.bodynodes.sensor.BnConstants;
import eu.bodynodes.sensor.BodynodesProtocol;
import eu.bodynodes.sensor.data.AppData;
import eu.bodynodes.sensor.data.BnSensorAppData;
import eu.bodynodes.sensor.R;
import eu.bodynodes.sensor.service.SensorServiceBluetooth;
import eu.bodynodes.sensor.service.SensorServiceWifi;

public class MainSensorActivity extends AppCompatActivity implements View.OnClickListener, View.OnTouchListener {

    private final static String TAG = "MainSensorActivity";

    private Button mStartButton;
    private Button mStopButton;
    private Button mSettingsButton;
    private Button mResetButton;
    private TextView mSensorNotSupportedPopup;
    private ProgressBar mProgressBar;
    private RelativeLayout mBodynodesKatana;
    private RelativeLayout mBodynodesGlove;
    private View mBodynodesThumb;
    private View mBodynodesIndexFinger;
    private View mBodynodesMiddleFinger;
    private View mBodynodesRingFinger;
    private View mBodynodesLittleFinger;
    private View mBodynodesPalm;
    private LinearLayout mSensortypeChecklist;
    private CheckBox mOrientationAbsCheckbox;
    private CheckBox mAccelerationRelCheckbox;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_sensor);

        getViewReferences();
        setOnClicks();
        updateUI();
        checkService();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BnConstants.ACTION_UPDATE_UI);
        intentFilter.addAction(BnConstants.ACTION_RECEIVED);
        LocalBroadcastManager.getInstance(this).registerReceiver(mSensorReceiver, intentFilter);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.FOREGROUND_SERVICE) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.FOREGROUND_SERVICE}, BnConstants.FOREGROUND_SERVICE_PERMISSION_CODE);
            }
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

    private final BroadcastReceiver mSensorReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if(intent.getAction().equals(BnConstants.ACTION_UPDATE_UI)) {
                updateUI();
            } else if(intent.getAction().equals(BnConstants.ACTION_RECEIVED)){
                //Toast.makeText(MainSensorActivity.this,intent.getStringExtra(BnConstants.INTENT_KEY_ACTION_BYTES),Toast.LENGTH_SHORT).show();
                String jsonText = intent.getStringExtra(BnConstants.KEY_JSON_ACTION);
                JSONObject actionJson;
                try {

                    actionJson = new JSONObject(jsonText);
                    if(!actionJson.has(BnConstants.ACTION_PLAYER_TAG)) {
                        return;
                    }
                    if(!actionJson.has(BnConstants.ACTION_BODYPART_TAG)) {
                        return;
                    }
                    if(!actionJson.has(BnConstants.ACTION_TYPE_TAG)) {
                        return;
                    }

                    String actionPlayer = actionJson.getString(BnConstants.ACTION_PLAYER_TAG);
                    String actionBodypart = actionJson.getString(BnConstants.ACTION_BODYPART_TAG);
                    String actionType = actionJson.getString(BnConstants.ACTION_TYPE_TAG);
                    if( !BnConstants.PLAYER_ALL_TAG.equals(actionPlayer) &&
                            !BnSensorAppData.getPlayerName(MainSensorActivity.this).equals(actionPlayer) ) {
                        Log.w(TAG,"Ignoring actions, it is for another player. Action player = " + actionPlayer +
                                " app player = " + BnSensorAppData.getPlayerName(MainSensorActivity.this));
                        return;
                    }
                    if( !BnConstants.BODYPART_ALL_TAG.equals(actionBodypart) &&
                            !BnSensorAppData.getBodypart(MainSensorActivity.this).equals(actionBodypart) ) {
                        Log.w(TAG,"Ignoring actions, it is for another bodypart. Action bodypart = " + actionBodypart +
                                " app bodypart = " + BnSensorAppData.getBodypart(MainSensorActivity.this));
                        return;
                    }

                    if (BnConstants.ACTION_TYPE_HAPTIC_TAG.equals(actionType)) {
                        Log.i(TAG,"Vibration action");
                        if (actionJson.has(BnConstants.ACTION_HAPTIC_STRENGTH_TAG) && actionJson.has(BnConstants.ACTION_HAPTIC_DURATIONMS_TAG)) {
                            vibrate(actionJson.getInt(BnConstants.ACTION_HAPTIC_DURATIONMS_TAG),
                                    actionJson.getInt(BnConstants.ACTION_HAPTIC_STRENGTH_TAG));
                        }
                    } else if(BnConstants.ACTION_TYPE_SETPLAYER_TAG.equals(actionType)){
                        Log.i(TAG,"New player action");
                        if( actionJson.has(BnConstants.ACTION_SETPLAYER_NEWPLAYER_TAG)) {
                            if (BnSensorAppData.getPlayerName(MainSensorActivity.this).equals(actionJson.getString(BnConstants.ACTION_SETPLAYER_NEWPLAYER_TAG))){
                                Log.w(TAG,"New player and local player are the same");
                                return;
                            }
                            BnSensorAppData.setPlayerName(MainSensorActivity.this, actionJson.getString(BnConstants.ACTION_SETPLAYER_NEWPLAYER_TAG) );
                        }
                    } else if(BnConstants.ACTION_TYPE_SETBODYPART_TAG.equals(actionType)) {
                        Log.i(TAG,"New bodypart action");

                        if( actionJson.has(BnConstants.ACTION_SETBODYPART_NEWBODYPART_TAG)) {
                            if (BnSensorAppData.getBodypart(MainSensorActivity.this).equals(actionJson.getString(BnConstants.ACTION_SETBODYPART_NEWBODYPART_TAG) )){
                                Log.w(TAG,"New bodypart and local bodypart are the same");
                                return;
                            }
                            BnSensorAppData.setBodypart(MainSensorActivity.this, actionJson.getString(BnConstants.ACTION_SETBODYPART_NEWBODYPART_TAG) );
                        }
                    } else if(BnConstants.ACTION_TYPE_ENABLESENSOR_TAG.equals(actionType)) {
                        Log.i(TAG,"Enable sensor action");
                        if(actionJson.has(BnConstants.ACTION_ENABLESENSOR_SENSORTYPE_TAG) && actionJson.has(BnConstants.ACTION_ENABLESENSOR_ENABLE_TAG) ){
                            if( actionJson.getString(BnConstants.ACTION_ENABLESENSOR_SENSORTYPE_TAG).equals(BnConstants.SENSORTYPE_ORIENTATION_ABS_TAG)) {
                                Log.i(TAG,"Orientation Abs sensor");
                                BnSensorAppData.setOrientationAbsSensorEnabled(MainSensorActivity.this,
                                        actionJson.getBoolean(BnConstants.ACTION_ENABLESENSOR_ENABLE_TAG));
                            } else if( actionJson.getString(BnConstants.ACTION_ENABLESENSOR_SENSORTYPE_TAG).equals(BnConstants.SENSORTYPE_ACCELERATION_REL_TAG)) {
                                Log.i(TAG,"Orientation Rel sensor");
                                BnSensorAppData.setAccelerationRelSensorEnabled(MainSensorActivity.this,
                                        actionJson.getBoolean(BnConstants.ACTION_ENABLESENSOR_ENABLE_TAG));
                            }
                        }
                    } else if(BnConstants.ACTION_TYPE_SETWIFI_TAG.equals(actionType)) {
                        if(actionJson.has(BnConstants.ACTION_SETWIFI_SSID_TAG) &&
                                actionJson.has(BnConstants.ACTION_SETWIFI_PASSWORD_TAG) &&
                                actionJson.has(BnConstants.ACTION_SETWIFI_MULTICAST_GROUP_TAG )){

                            BnSensorAppData.setMulticastGroup(MainSensorActivity.this, actionJson.getString(BnConstants.ACTION_SETWIFI_MULTICAST_GROUP_TAG ) );
                            Log.i(TAG,"Orientation Rel sensor");

                        }
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        }
    };

    @Override
    protected void onDestroy() {
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mSensorReceiver);
        super.onDestroy();
    }

    @Override
    protected void onResume() {
        super.onResume();

        mOrientationAbsCheckbox.setChecked(BnSensorAppData.isOrientationAbsSensorEnabled(this));
        mAccelerationRelCheckbox.setChecked(BnSensorAppData.isAccelerationRelSensorEnabled(this));
        if( AppData.isCommunicationWifi(this) ){
            mStartButton.setText(R.string.main_sensor_page_start_wifi_button_text);
            mStopButton.setText(R.string.main_sensor_page_stop_wifi_button_text);
        } else if( AppData.isCommunicationBluetooth(this) ){
            mStartButton.setText(R.string.main_sensor_page_start_bluetooth_button_text);
            mStopButton.setText(R.string.main_sensor_page_stop_bluetooth_button_text);
        }

    }

    private void getViewReferences() {
        mStartButton = findViewById(R.id.main_sensor_start_button);
        mStopButton = findViewById(R.id.main_sensor_stop_button);
        mSettingsButton = findViewById(R.id.main_sensor_settings_button);
        mResetButton = findViewById(R.id.main_sensor_reset_button);
        mProgressBar = findViewById(R.id.main_sensor_progress_bar);

        mBodynodesKatana = findViewById(R.id.main_sensor_katana_layout);
        mBodynodesGlove = findViewById(R.id.main_sensor_glove_layout);
        mBodynodesThumb = findViewById(R.id.main_sensor_thumb_right);
        mBodynodesIndexFinger = findViewById(R.id.main_sensor_index_finger_right);
        mBodynodesMiddleFinger = findViewById(R.id.main_sensor_middle_finger_right);
        mBodynodesRingFinger = findViewById(R.id.main_sensor_ring_finger_right);
        mBodynodesLittleFinger = findViewById(R.id.main_sensor_little_finger_right);
        mBodynodesPalm = findViewById(R.id.main_sensor_palm_right);

        mSensorNotSupportedPopup = findViewById(R.id.katana_page_sensor_not_supported_popup);
        mSensortypeChecklist = findViewById(R.id.main_sensor_sensortype_checklist);
        mOrientationAbsCheckbox = findViewById(R.id.main_sensor_orientation_abs_checkbox);
        mAccelerationRelCheckbox = findViewById(R.id.main_sensor_acceleration_rel_checkbox);

        if(AppData.isServiceRunning()){
            sensorServiceONUI();
        } else {
            sensorServiceOFFUI();
        }
    }

    private void setOnClicks() {
        mStopButton.setOnClickListener(this);
        mStartButton.setOnClickListener(this);
        mSettingsButton.setOnClickListener(this);
        mResetButton.setOnClickListener(this);

        mBodynodesThumb.setOnTouchListener(this);
        mBodynodesIndexFinger.setOnTouchListener(this);
        mBodynodesMiddleFinger.setOnTouchListener(this);
        mBodynodesRingFinger.setOnTouchListener(this);
        mBodynodesLittleFinger.setOnTouchListener(this);
        mBodynodesPalm.setOnTouchListener(this);
    }

    private void startSersorService(){
        BnSensorAppData.setOrientationAbsSensorEnabled(this, mOrientationAbsCheckbox.isChecked());
        BnSensorAppData.setAccelerationRelSensorEnabled(this, mAccelerationRelCheckbox.isChecked());

        Intent sensorServiceIntent = null;
        if(AppData.getCommunicationType(this) == BnConstants.COMMUNICATION_TYPE_WIFI) {
            sensorServiceIntent = new Intent(this, SensorServiceWifi.class);
        } else if(AppData.getCommunicationType(this) == BnConstants.COMMUNICATION_TYPE_BLUETOOTH) {
            sensorServiceIntent = new Intent(this, SensorServiceBluetooth.class);
        } else {
            Log.d(TAG, "Cannot start because communication type is not implemented");
            return;
        }
        if(sensorServiceIntent != null) {
            startService(sensorServiceIntent);
            sensorServiceONUI();
        }
    }

    private void sensorServiceONUI() {
        mSensortypeChecklist.setVisibility(View.GONE);
        mStartButton.setVisibility(View.GONE);
        mSettingsButton.setVisibility(View.GONE);
        mResetButton.setVisibility(View.VISIBLE);
        mBodynodesGlove.setVisibility(View.VISIBLE);
    }

    private void stopSersorService(){
        if(AppData.getCommunicationType(this) == BnConstants.COMMUNICATION_TYPE_WIFI) {
            stopService(new Intent(this, SensorServiceWifi.class));
        } else if(AppData.getCommunicationType(this) == BnConstants.COMMUNICATION_TYPE_BLUETOOTH) {
            stopService(new Intent(this, SensorServiceBluetooth.class));
        }
        sensorServiceOFFUI();
    }

    private void sensorServiceOFFUI() {
        mSensortypeChecklist.setVisibility(View.VISIBLE);
        mStopButton.setVisibility(View.GONE);
        mSettingsButton.setVisibility(View.VISIBLE);
        mResetButton.setVisibility(View.GONE);
        mBodynodesGlove.setVisibility(View.VISIBLE);
    }

    private void checkService() {
        mProgressBar.setVisibility(View.GONE);
        if (AppData.isServiceRunning()) {
            mStartButton.setVisibility(View.GONE);
            mSensortypeChecklist.setVisibility(View.GONE);
            mStopButton.setVisibility(View.VISIBLE);
        } else {
            mStartButton.setVisibility(View.VISIBLE);
            mSensortypeChecklist.setVisibility(View.VISIBLE);
            mStopButton.setVisibility(View.GONE);
        }
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        //if(!AppData.isCommunicationConnected()){
        //  return false;
        //}

        int id = view.getId();
        int action = motionEvent.getAction();
        if( action != MotionEvent.ACTION_DOWN && action != MotionEvent.ACTION_UP ) {
            Log.i(TAG, "Not an action down or action up");
            return false;
        }
        int sending = 0;
        if( action == MotionEvent.ACTION_DOWN ) {
            sending = 1;
        }

        int[] intArray;
        Intent intent;
        switch (id){
            case R.id.main_sensor_index_finger_right:
                intArray = new int[]{90, 90, 90, 90, 90, sending, 0, 0, 0};
                intent = new Intent(BnConstants.ACTION_GLOVE_SENSOR_MESSAGE);
                intent.putExtra(BnConstants.GLOVE_SENSOR_DATA, intArray);
                LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
                return true;
            case R.id.main_sensor_middle_finger_right:
                intArray = new int[]{90, 90, 90, 90, 90, 0, sending, 0, 0};
                intent = new Intent(BnConstants.ACTION_GLOVE_SENSOR_MESSAGE);
                intent.putExtra(BnConstants.GLOVE_SENSOR_DATA, intArray);
                LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
                return true;
            case R.id.main_sensor_ring_finger_right:
                intArray = new int[]{90, 90, 90, 90, 90, 0, 0, sending, 0};
                intent = new Intent(BnConstants.ACTION_GLOVE_SENSOR_MESSAGE);
                intent.putExtra(BnConstants.GLOVE_SENSOR_DATA, intArray);
                LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
                return true;
            case R.id.main_sensor_little_finger_right:
                intArray = new int[]{90, 90, 90, 90, 90, 0, 0, 0, sending};
                intent = new Intent(BnConstants.ACTION_GLOVE_SENSOR_MESSAGE);
                intent.putExtra(BnConstants.GLOVE_SENSOR_DATA, intArray);
                LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
                return true;
            case R.id.main_sensor_thumb_right:
            case R.id.main_sensor_palm_right:
            default:
                break;
        }
        return false;
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();
        Intent intent = null;
        switch (id){
            case R.id.main_sensor_start_button:
                if(!mOrientationAbsCheckbox.isChecked() && !mAccelerationRelCheckbox.isChecked()){
                    Toast.makeText(this, R.string.main_sensor_select_sensortype, Toast.LENGTH_SHORT).show();
                    return;
                }

                if (AppData.isCommunicationWifi(this) &&
                        ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, BnConstants.WIFI_PERMISSION_CODE);
                } else if(AppData.isCommunicationBluetooth(this) && Build.VERSION.SDK_INT >= Build.VERSION_CODES.S &&
                        ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED){
                    ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.BLUETOOTH_CONNECT}, BnConstants.BLUETOOTH_PERMISSION_CODE);
                } else {
                    startSersorService();
                }
                break;
            case R.id.main_sensor_stop_button:
                stopSersorService();
                break;
            case R.id.main_sensor_settings_button:
                intent = new Intent(this, SettingsActivity.class);
                startActivity(intent);
                break;
            case R.id.main_sensor_reset_button:
                intent = new Intent(BnConstants.ACTION_RESET_MESSAGE);
                LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
            default:
                break;
        }

        mProgressBar.setVisibility(View.VISIBLE);
        new Handler().postDelayed(() -> {
            checkService();
            updateUI();
        }, 1000);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == BnConstants.WIFI_PERMISSION_CODE &&
                permissions[0].equals(Manifest.permission.ACCESS_FINE_LOCATION)){
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                startSersorService();
            } else {
                Toast.makeText(this, R.string.wifi_permission_not_granted, Toast.LENGTH_SHORT).show();
            }
        } else if(requestCode == BnConstants.FOREGROUND_SERVICE_PERMISSION_CODE) {
            if (grantResults.length > 0 && grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                Toast.makeText(this, R.string.notification_permission_not_granted, Toast.LENGTH_SHORT).show();
            }
        }
    }

    private void updateUI() {
        runOnUiThread(() -> {
            SensorManager sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
            if (sensorManager != null) {
                Sensor sensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
                if (sensor == null) {
                    mSensorNotSupportedPopup.setVisibility(View.VISIBLE);
                    return;
                }
            }
            mOrientationAbsCheckbox.setChecked(BnSensorAppData.isOrientationAbsSensorEnabled(this));
            mAccelerationRelCheckbox.setChecked(BnSensorAppData.isAccelerationRelSensorEnabled(this));

            if(!AppData.isServiceRunning()) {
                mBodynodesKatana.setBackgroundResource(R.drawable.bg_katana_grey);
                mBodynodesThumb.setBackgroundColor(getResources().getColor(R.color.btc_grey));
                mBodynodesIndexFinger.setBackgroundColor(getResources().getColor(R.color.btc_grey));
                mBodynodesMiddleFinger.setBackgroundColor(getResources().getColor(R.color.btc_grey));
                mBodynodesRingFinger.setBackgroundColor(getResources().getColor(R.color.btc_grey));
                mBodynodesLittleFinger.setBackgroundColor(getResources().getColor(R.color.btc_grey));
                mBodynodesPalm.setBackgroundColor(getResources().getColor(R.color.btc_grey));
            } else if(AppData.isCommunicationConnected()) {
                mBodynodesKatana.setBackgroundResource(R.drawable.bg_katana_green);
                mBodynodesThumb.setBackgroundColor(getResources().getColor(R.color.btc_green));
                mBodynodesIndexFinger.setBackgroundColor(getResources().getColor(R.color.btc_green));
                mBodynodesMiddleFinger.setBackgroundColor(getResources().getColor(R.color.btc_green));
                mBodynodesRingFinger.setBackgroundColor(getResources().getColor(R.color.btc_green));
                mBodynodesLittleFinger.setBackgroundColor(getResources().getColor(R.color.btc_green));
                mBodynodesPalm.setBackgroundColor(getResources().getColor(R.color.btc_green));
            } else if(AppData.isCommunicationWaitingACK()){
                mBodynodesKatana.setBackgroundResource(R.drawable.bg_katana_yellow);
                mBodynodesThumb.setBackgroundColor(getResources().getColor(R.color.btc_yellow));
                mBodynodesIndexFinger.setBackgroundColor(getResources().getColor(R.color.btc_yellow));
                mBodynodesMiddleFinger.setBackgroundColor(getResources().getColor(R.color.btc_yellow));
                mBodynodesRingFinger.setBackgroundColor(getResources().getColor(R.color.btc_yellow));
                mBodynodesLittleFinger.setBackgroundColor(getResources().getColor(R.color.btc_yellow));
                mBodynodesPalm.setBackgroundColor(getResources().getColor(R.color.btc_yellow));
            } else {
                mBodynodesKatana.setBackgroundResource(R.drawable.bg_katana_red);
                mBodynodesThumb.setBackgroundColor(getResources().getColor(R.color.btc_red));
                mBodynodesIndexFinger.setBackgroundColor(getResources().getColor(R.color.btc_red));
                mBodynodesMiddleFinger.setBackgroundColor(getResources().getColor(R.color.btc_red));
                mBodynodesRingFinger.setBackgroundColor(getResources().getColor(R.color.btc_red));
                mBodynodesLittleFinger.setBackgroundColor(getResources().getColor(R.color.btc_red));
                mBodynodesPalm.setBackgroundColor(getResources().getColor(R.color.btc_red));
            }
            mBodynodesKatana.invalidate();
            mBodynodesThumb.invalidate();
            mBodynodesIndexFinger.invalidate();
            mBodynodesMiddleFinger.invalidate();
            mBodynodesRingFinger.invalidate();
            mBodynodesLittleFinger.invalidate();
            mBodynodesPalm.invalidate();
        });
    }

}
