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

package eu.bodynodes.sensor.data;

import android.content.Context;
import android.content.SharedPreferences;

import eu.bodynodes.sensor.BnConstants;

public class AppData {

    private static boolean sIsServiceRunning = false;
    private static int sCommunicationState = BnConstants.COMMUNICATION_STATE_DISCONNECTED;

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
        setCommunicationState(BnConstants.COMMUNICATION_STATE_CONNECTED);
    }

    public static void setCommunicationDisconnected() {
        setCommunicationState(BnConstants.COMMUNICATION_STATE_DISCONNECTED);
    }

    public static void setCommunicationWaitingACK() {
        setCommunicationState(BnConstants.COMMUNICATION_STATE_WAITING_ACK);
    }

    public static boolean isCommunicationWaitingACK() {
        return sCommunicationState == BnConstants.COMMUNICATION_STATE_WAITING_ACK;
    }

    public static boolean isCommunicationConnected() {
        return sCommunicationState == BnConstants.COMMUNICATION_STATE_CONNECTED;
    }

    public static boolean isCommunicationDisconnected() {
        return sCommunicationState == BnConstants.COMMUNICATION_STATE_DISCONNECTED;
    }

    public static int getCommunicationType(Context context){
        SharedPreferences sharedPref = context.getSharedPreferences(BnConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getInt(BnConstants.COMMUNICATION_TYPE, BnConstants.COMMUNICATION_TYPE_WIFI);
    }

    public static void setCommunitcationType(Context context, int communicationType) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putInt(BnConstants.COMMUNICATION_TYPE, communicationType);
        editor.apply();
    }

    public static boolean isCommunicationWifi(Context context) {
        int communicationType = getCommunicationType(context);
        return communicationType == BnConstants.COMMUNICATION_TYPE_WIFI;
    }

    public static boolean isCommunicationBluetooth(Context context) {
        int communicationType = getCommunicationType(context);
        return communicationType == BnConstants.COMMUNICATION_TYPE_BLUETOOTH;
    }

    public static int getSensorIntervalMs(Context context) {
        SharedPreferences sharedPref = context.getSharedPreferences(BnConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getInt(BnConstants.SENSOR_INTERVAL_MS, BnConstants.SENSOR_READ_INTERVAL_MS);
    }

    public static void setSensorIntervalMs(Context context, int sensorIntervalMs) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putInt(BnConstants.SENSOR_INTERVAL_MS, sensorIntervalMs);
        editor.apply();
    }

}
