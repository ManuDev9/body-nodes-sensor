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
 
package eu.bodynodesdev.sensor;


import eu.bodynodesdev.common.BnConstants;
import eu.bodynodesdev.sensor.data.AppData;

public class BnAppConstants {


    public static final String PLAYER_NAME_DEFAULT = "1";

    public final static String NOTIFICATION_CHANNEL_ID = "eu.bodynodesdev.sensor";
    public final static String NOTIFICATION_CHANNEL_NAME = "Bodynodes Service";

    public static final int  COMMUNICATION_TYPE_WIFI = 0;
    public static final int  COMMUNICATION_TYPE_BLUETOOTH = 1;
    public static final int  COMMUNICATION_TYPE_BLE = 2;

    public static final float BIG_ORIENTATION_ABS_DIFF = 0.04f;

    public static final int WIFI_PERMISSION_CODE                = 10000;
    public static final int FOREGROUND_SERVICE_PERMISSION_CODE  = 10001;
    public static final int SENSOR_SERVICE_NOTIFICATION_ID      = 10002;
    public static final int BLUETOOTH_PERMISSION_CODE           = 10003;

    public static final String ACTION_UPDATE_UI = "eu.bodynodesdev.sensor.ACTION_UPDATE_UI";
    public static final String ACTION_GLOVE_SENSOR_MESSAGE = "eu.bodynodesdev.sensor.ACTION_GLOVE_SENSOR_MESSAGE";
    public static final String ACTION_RECEIVED = "eu.bodynodesdev.sensor.ACTION_RECEIVED";
    public static final String KEY_JSON_ACTION = "JSON_ACTION";
    
    public static final int[] REORIENT_IO_AXIS = new int[]{ 0, 2, 3, 1 };
    public static final int[] REORIENT_IO_SIGN = new int[]{ 1, 1, -1, 1 };

    // SHARED REFS TAGS
    public static final String BODYNODES_SHARED_PREFS = "BODYNODESDEV_SHARED_PREFS";
    public static final String LOCAL_PORT_IN_NUMBER = "LOCAL_PORT_IN_NUMBER";
    public static final String REMOTE_PORT_OUT_NUMBER = "REMOTE_PORT_OUT_NUMBER";
    public static final String COMMUNICATION_TYPE = "COMMUNICATION_TYPE";
    public static final String BODYPART = "BODYPART";
    public static final String GLOVE_BODYPART = "GLOVE_BODYPART";
    public static final String GLOVE_SENSOR_DATA = "GLOVE_SENSOR_DATA";
    public static final String ORIENTATION_ABSOLUTE_ENABLED = "ORIENTATION_ABSOLUTE_ENABLED";
    public static final String ACCELERATION_RELATIVE_ENABLED = "ACCELERATION_RELATIVE_ENABLED";
    public static final String SENSOR_INTERVAL_MS = "SENSOR_INTERVAL_MS";
    public static final String PLAYER_NAME = "PLAYER_NAME";
    public static final String MULTICAST_GROUP = "MULTICAST_GROUP";


    public static final long CONNECTION_KEEP_ALIVE_SEND_INTERVAL_MS = 30000;

    public static final long CONNECTION_KEEP_ALIVE_REC_INTERVAL_MS = 60000;

    public static final long CONNECTION_ACK_INTERVAL_MS = 5000;

    public static final int SENSOR_READ_INTERVAL_MS = 30;

    public static final int COMMUNICATION_STATE_DISCONNECTED = 0;
    public static final int COMMUNICATION_STATE_CONNECTED = 1;
    public static final int COMMUNICATION_STATE_WAITING_ACK = 2;

}



