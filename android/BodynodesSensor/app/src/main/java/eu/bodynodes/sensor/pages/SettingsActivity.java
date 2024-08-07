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

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;

import eu.bodynodes.sensor.BodynodesConstants;
import eu.bodynodes.sensor.R;
import eu.bodynodes.sensor.data.AppData;
import eu.bodynodes.sensor.data.BodynodesData;

public class SettingsActivity extends AppCompatActivity implements View.OnClickListener {

    private EditText mPlayerNameEdit;
    private EditText mBodypartEdit;
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
        if(AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_BLUETOOTH) {
            mConnectionTypeBluetoothRadio.setChecked(true);
            mConnectionTypeWifiRadio.setChecked(false);
        } else if(AppData.getCommunicationType(this) == BodynodesConstants.COMMUNICATION_TYPE_WIFI){
            mConnectionTypeBluetoothRadio.setChecked(false);
            mConnectionTypeWifiRadio.setChecked(true);
        }

        if( BodynodesConstants.BODY_HAND_LEFT_TAG.equals(BodynodesData.getGloveBodypart(this)) ) {
            mGloveBodypartLeftHandRadio.setChecked(true);
            mGloveBodypartRightHandRadio.setChecked(false);
        } else if( BodynodesConstants.BODY_HAND_RIGHT_TAG.equals(BodynodesData.getGloveBodypart(this)) ){
            mGloveBodypartLeftHandRadio.setChecked(false);
            mGloveBodypartRightHandRadio.setChecked(true);
        }

        mPlayerNameEdit.setText(BodynodesData.getPlayerName(this));
        mBodypartEdit.setText(BodynodesData.getBodypart(this));
        mSensorIntervalMsEdit.setText(AppData.getSensorIntervalMs(this)+"");
    }

    private void setOnClicks() {
        mSaveButton.setOnClickListener(this);
    }

    private void getViewReferences() {
        mPlayerNameEdit = findViewById(R.id.settings_player_name_edit);
        mBodypartEdit = findViewById(R.id.settings_bodypart_edit);
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
                BodynodesData.setPlayerName(this, mPlayerNameEdit.getText().toString());
                BodynodesData.setBodypart(this, mBodypartEdit.getText().toString());
                AppData.setSensorIntervalMs(this, Integer.parseInt(mSensorIntervalMsEdit.getText().toString()));
                if(mConnectionTypeBluetoothRadio.isChecked()){
                    AppData.setCommunitcationType(this, BodynodesConstants.COMMUNICATION_TYPE_BLUETOOTH);
                } else if(mConnectionTypeWifiRadio.isChecked()){
                    AppData.setCommunitcationType(this, BodynodesConstants.COMMUNICATION_TYPE_WIFI);
                }

                if(mGloveBodypartLeftHandRadio.isChecked()){
                    BodynodesData.setGloveBodypart(this, BodynodesConstants.BODY_HAND_LEFT_TAG);
                } else if(mGloveBodypartRightHandRadio.isChecked()){
                    BodynodesData.setGloveBodypart(this, BodynodesConstants.BODY_HAND_RIGHT_TAG);
                }

                finish();
                break;
        }
    }
}