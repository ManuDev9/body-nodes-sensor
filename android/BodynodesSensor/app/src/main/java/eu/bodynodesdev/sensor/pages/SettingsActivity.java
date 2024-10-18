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

package eu.bodynodesdev.sensor.pages;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;

import eu.bodynodesdev.common.BnConstants;
import eu.bodynodesdev.sensor.BnAppConstants;
import eu.bodynodesdev.sensor.R;
import eu.bodynodesdev.sensor.data.AppData;
import eu.bodynodesdev.sensor.data.BnSensorAppData;

public class SettingsActivity extends AppCompatActivity implements View.OnClickListener {

    private EditText mPlayerNameEdit;
    private EditText mBodypartEdit;
    private EditText mWifiMulticastGroupEdit;
    private EditText mSensorIntervalMsEdit;
    private RadioButton mGloveBodypartLeftHandRadio;
    private RadioButton mGloveBodypartRightHandRadio;
    private RadioButton mConnectionTypeBluetoothRadio;
    private RadioButton mConnectionTypeWifiRadio;
    private Button mSaveButton;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        getViewReferences();
        setOnClicks();
        initUIfromData();
    }

    private void initUIfromData() {
        if(AppData.getCommunicationType(this) == BnAppConstants.COMMUNICATION_TYPE_BLUETOOTH) {
            mConnectionTypeBluetoothRadio.setChecked(true);
            mConnectionTypeWifiRadio.setChecked(false);
        } else if(AppData.getCommunicationType(this) == BnAppConstants.COMMUNICATION_TYPE_WIFI){
            mConnectionTypeBluetoothRadio.setChecked(false);
            mConnectionTypeWifiRadio.setChecked(true);
        }

        if( BnConstants.BODYPART_HAND_LEFT_TAG.equals(BnSensorAppData.getGloveBodypart(this)) ) {
            mGloveBodypartLeftHandRadio.setChecked(true);
            mGloveBodypartRightHandRadio.setChecked(false);
        } else if( BnConstants.BODYPART_HAND_RIGHT_TAG.equals(BnSensorAppData.getGloveBodypart(this)) ){
            mGloveBodypartLeftHandRadio.setChecked(false);
            mGloveBodypartRightHandRadio.setChecked(true);
        }

        mPlayerNameEdit.setText(BnSensorAppData.getPlayerName(this));
        mBodypartEdit.setText(BnSensorAppData.getBodypart(this));
        mWifiMulticastGroupEdit.setText(BnSensorAppData.getMulticastGroup(this));
        mSensorIntervalMsEdit.setText(AppData.getSensorIntervalMs(this)+"");
    }

    private void setOnClicks() {
        mSaveButton.setOnClickListener(this);
    }

    private void getViewReferences() {
        mPlayerNameEdit = findViewById(R.id.settings_player_name_edit);
        mBodypartEdit = findViewById(R.id.settings_bodypart_edit);
        mWifiMulticastGroupEdit = findViewById(R.id.wifi_multicast_group_edit);
        mSensorIntervalMsEdit = findViewById(R.id.settings_sensor_interval_ms_edit);
        mGloveBodypartLeftHandRadio = findViewById(R.id.settings_connection_type_glovebodypart_left_radio);
        mGloveBodypartRightHandRadio = findViewById(R.id.settings_connection_type_glovebodypart_right_radio);
        mConnectionTypeBluetoothRadio = findViewById(R.id.settings_connection_type_bluetooth_radio);
        mConnectionTypeWifiRadio = findViewById(R.id.settings_connection_type_wifi_radio);
        mSaveButton = findViewById(R.id.settings_save_button);
    }

    @Override
    public void onClick(View view) {
        int id = view.getId();
        switch (id) {
            case R.id.settings_save_button:
                BnSensorAppData.setPlayerName(this, mPlayerNameEdit.getText().toString());
                BnSensorAppData.setBodypart(this, mBodypartEdit.getText().toString());
                BnSensorAppData.setMulticastGroup(this, mWifiMulticastGroupEdit.getText().toString());
                AppData.setSensorIntervalMs(this, Integer.parseInt(mSensorIntervalMsEdit.getText().toString()));
                if(mConnectionTypeBluetoothRadio.isChecked()){
                    AppData.setCommunitcationType(this, BnAppConstants.COMMUNICATION_TYPE_BLUETOOTH);
                } else if(mConnectionTypeWifiRadio.isChecked()){
                    AppData.setCommunitcationType(this, BnAppConstants.COMMUNICATION_TYPE_WIFI);
                }

                if(mGloveBodypartLeftHandRadio.isChecked()){
                    BnSensorAppData.setGloveBodypart(this, BnConstants.BODYPART_HAND_LEFT_TAG);
                } else if(mGloveBodypartRightHandRadio.isChecked()){
                    BnSensorAppData.setGloveBodypart(this, BnConstants.BODYPART_HAND_RIGHT_TAG);
                }

                finish();
                break;
        }
    }
}