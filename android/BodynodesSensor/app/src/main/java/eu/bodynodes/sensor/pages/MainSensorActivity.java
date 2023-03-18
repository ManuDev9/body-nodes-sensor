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
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import eu.bodynodes.sensor.BodynodesConstants;
import eu.bodynodes.sensor.data.AppData;
import eu.bodynodes.sensor.data.BodynodesData;
import eu.bodynodes.sensor.R;
import eu.bodynodes.sensor.service.SensorServiceWifiOsc;
import eu.bodynodes.sensor.service.SensorServiceWifi;

public class MainSensorActivity extends AppCompatActivity implements View.OnClickListener {

    private final static String TAG = "MainSensorActivity";

    private Button mStartButton;
    private Button mSettingsButton;
    private Button mStopButton;
    private TextView mSensorNotSupportedPopup;
    private ProgressBar mProgressBar;
    private RelativeLayout mBodyKatanaLayout;
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
        intentFilter.addAction(BodynodesConstants.ACTION_UPDATE_UI);
        intentFilter.addAction(BodynodesConstants.ACTION_RECEIVED);
        LocalBroadcastManager.getInstance(this).registerReceiver(mSensorReceiver, intentFilter);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.FOREGROUND_SERVICE) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.FOREGROUND_SERVICE}, BodynodesConstants.FOREGROUND_SERVICE_PERMISSION_CODE);
            }
        }
    }

    private final BroadcastReceiver mSensorReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if(intent.getAction().equals(BodynodesConstants.ACTION_UPDATE_UI)) {
                updateUI();
            } else if(intent.getAction().equals(BodynodesConstants.ACTION_RECEIVED)){
                Toast.makeText(MainSensorActivity.this,intent.getStringExtra(BodynodesConstants.KEY_JSON_ACTION),Toast.LENGTH_SHORT).show();
            }
        }
    };

    @Override
    protected void onDestroy() {
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mSensorReceiver);
        super.onDestroy();
    }

    private void getViewReferences() {
        mStartButton = findViewById(R.id.main_sensor_start_button);
        mStopButton = findViewById(R.id.main_sensor_stop_button);
        mSettingsButton = findViewById(R.id.main_sensor_settings_button);
        mProgressBar = findViewById(R.id.main_sensor_progress_bar);
        mBodyKatanaLayout = findViewById(R.id.main_sensor_katana_layout);
        mSensorNotSupportedPopup = findViewById(R.id.katana_page_sensor_not_supported_popup);
        mSensortypeChecklist = findViewById(R.id.main_sensor_sensortype_checklist);
        mOrientationAbsCheckbox = findViewById(R.id.main_sensor_orientation_abs_checkbox);
        mAccelerationRelCheckbox = findViewById(R.id.main_sensor_acceleration_rel_checkbox);

        mOrientationAbsCheckbox.setChecked(BodynodesData.isOrientationAbsSensorEnabled(this));
        mAccelerationRelCheckbox.setChecked(BodynodesData.isAccelerationRelSensorEnabled(this));
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
    }

    private void startSersorService(){
        BodynodesData.setOrientationAbsSensorEnabled(this, mOrientationAbsCheckbox.isChecked());
        BodynodesData.setAccelerationRelSensorEnabled(this, mAccelerationRelCheckbox.isChecked());

        Intent sensorServiceIntent;
        if(AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_WIFI) {
            sensorServiceIntent = new Intent(this, SensorServiceWifi.class);
        } else if(AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_WIFI_OSC) {
            sensorServiceIntent = new Intent(this, SensorServiceWifiOsc.class);
        } else {
            Log.d(TAG, "Cannot start because communication type is not implemented");
            return;
        }

        startService(sensorServiceIntent);
        sensorServiceONUI();
    }

    private void sensorServiceONUI() {
        mSensortypeChecklist.setVisibility(View.GONE);
        mStartButton.setVisibility(View.GONE);
        mSettingsButton.setVisibility(View.GONE);
    }

    private void stopSersorService(){
        if(AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_WIFI) {
            stopService(new Intent(this, SensorServiceWifi.class));
        } else if(AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_WIFI_OSC) {
            stopService(new Intent(this, SensorServiceWifiOsc.class));
        }
        sensorServiceOFFUI();
    }

    private void sensorServiceOFFUI() {
        mSensortypeChecklist.setVisibility(View.VISIBLE);
        mStopButton.setVisibility(View.GONE);
        mSettingsButton.setVisibility(View.VISIBLE);
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
    public void onClick(View view) {
        int id = view.getId();
        switch (id){
            case R.id.main_sensor_start_button:
                if(!mOrientationAbsCheckbox.isChecked() && !mAccelerationRelCheckbox.isChecked()){
                    Toast.makeText(this, R.string.main_sensor_select_sensortype, Toast.LENGTH_SHORT).show();
                    return;
                }

                if (AppData.isCommunicationWifi(this) &&
                        ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(this, new String[] { Manifest.permission.ACCESS_FINE_LOCATION }, BodynodesConstants.WIFI_PERMISSION_CODE);
                } else {
                    startSersorService();
                }
                break;
            case R.id.main_sensor_stop_button:
                stopSersorService();
                break;
            case R.id.main_sensor_settings_button:
                Intent intent = new Intent(this, SettingsActivity.class);
                startActivity(intent);
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
        if (requestCode == BodynodesConstants.WIFI_PERMISSION_CODE &&
                permissions[0].equals(Manifest.permission.ACCESS_FINE_LOCATION)){
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                startSersorService();
            } else {
                Toast.makeText(this, R.string.wifi_permission_not_granted, Toast.LENGTH_SHORT).show();
            }
        } else if(requestCode == BodynodesConstants.FOREGROUND_SERVICE_PERMISSION_CODE) {
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
            if(!AppData.isServiceRunning()) {
                mBodyKatanaLayout.setBackgroundResource(R.drawable.bg_katana_grey);
            } else if(AppData.isCommunicationConnected()) {
                mBodyKatanaLayout.setBackgroundResource(R.drawable.bg_katana_green);
            } else if(AppData.isCommunicationWaitingACK()){
                mBodyKatanaLayout.setBackgroundResource(R.drawable.bg_katana_yellow);
            } else {
                mBodyKatanaLayout.setBackgroundResource(R.drawable.bg_katana_red);
            }
            mBodyKatanaLayout.invalidate();
        });
    }
}
