/*
 * MIT License
 *
 * Copyright (c) 2019-2025 Manuel Bottini
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

package eu.bodynodesdev.sensor.data;

import android.content.Context;
import android.content.SharedPreferences;

import eu.bodynodesdev.common.BnConstants;
import eu.bodynodesdev.sensor.BnAppConstants;

public class AppData {

    private static boolean sIsServiceRunning = false;
    private static int sCommunicationState = BnAppConstants.COMMUNICATION_STATE_DISCONNECTED;

    public static boolean isServiceRunning(){
        return sIsServiceRunning;
    }
    public static void setServiceRunning(boolean is_it) {
        sIsServiceRunning = is_it;
    }

    public static void setCommunicationState(int state){
        sCommunicationState = state;
    }

    public static void setCommunicationConnected() {
        setCommunicationState(BnAppConstants.COMMUNICATION_STATE_CONNECTED);
    }

    public static void setCommunicationDisconnected() {
        setCommunicationState(BnAppConstants.COMMUNICATION_STATE_DISCONNECTED);
    }

    public static void setCommunicationWaitingACK() {
        setCommunicationState(BnAppConstants.COMMUNICATION_STATE_WAITING_ACK);
    }

    public static boolean isCommunicationWaitingACK() {
        return sCommunicationState == BnAppConstants.COMMUNICATION_STATE_WAITING_ACK;
    }

    public static boolean isCommunicationConnected() {
        return sCommunicationState == BnAppConstants.COMMUNICATION_STATE_CONNECTED;
    }

    public static boolean isCommunicationDisconnected() {
        return sCommunicationState == BnAppConstants.COMMUNICATION_STATE_DISCONNECTED;
    }

    public static int getCommunicationType(Context context){
        SharedPreferences sharedPref = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getInt(BnAppConstants.COMMUNICATION_TYPE, BnAppConstants.COMMUNICATION_TYPE_WIFI);
    }

    public static void setCommunitcationType(Context context, int communicationType) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putInt(BnAppConstants.COMMUNICATION_TYPE, communicationType);
        editor.apply();
    }

    public static boolean isCommunicationWifi(Context context) {
        int communicationType = getCommunicationType(context);
        return communicationType == BnAppConstants.COMMUNICATION_TYPE_WIFI;
    }

    public static boolean isCommunicationBluetooth(Context context) {
        int communicationType = getCommunicationType(context);
        return communicationType == BnAppConstants.COMMUNICATION_TYPE_BLUETOOTH;
    }

    public static int getSensorIntervalMs(Context context) {
        SharedPreferences sharedPref = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getInt(BnAppConstants.SENSOR_INTERVAL_MS, BnAppConstants.SENSOR_READ_INTERVAL_MS);
    }

    public static void setSensorIntervalMs(Context context, int sensorIntervalMs) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putInt(BnAppConstants.SENSOR_INTERVAL_MS, sensorIntervalMs);
        editor.apply();
    }

}
