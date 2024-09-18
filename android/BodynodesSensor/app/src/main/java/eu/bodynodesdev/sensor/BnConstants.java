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

package eu.bodynodesdev.sensor;

/**
 * Created by m.bottini on 04/12/2015.
 */
public class BnConstants {


    public static final String PLAYER_ALL_TAG           = "all";
    public static final String PLAYER_NONE_TAG          = "none";

    public final static String NOTIFICATION_CHANNEL_ID = "eu.bodynodesdev.sensor";
    public final static String NOTIFICATION_CHANNEL_NAME = "Bodynodes Service";

    public static final String ACTION_UPDATE_UI = "eu.bodynodesdev.sensor.ACTION_UPDATE_UI";
    public static final String ACTION_GLOVE_SENSOR_MESSAGE = "eu.bodynodesdev.sensor.ACTION_GLOVE_SENSOR_MESSAGE";
    public static final String ACTION_RESET_MESSAGE = "eu.bodynodesdev.sensor.ACTION_RESET_MESSAGE";
    public static final String ACTION_RECEIVED = "eu.bodynodesdev.sensor.ACTION_RECEIVED";
    public static final String KEY_JSON_ACTION = "JSON_ACTION";

    /*Body Parts*/
    public static final String BODYPART_NONE_TAG              = "none";
    public static final String BODYPART_HEAD_TAG              = "head";
    public static final String BODYPART_HAND_LEFT_TAG         = "hand_left";
    public static final String BODYPART_LOWERARM_LEFT_TAG     = "lowerarm_left";
    public static final String BODYPART_UPPERARM_LEFT_TAG     = "upperarm_left";
    public static final String BODYPART_BODY_TAG              = "body";
    public static final String BODYPART_LOWERARM_RIGHT_TAG    = "lowerarm_right";
    public static final String BODYPART_UPPERARM_RIGHT_TAG    = "upperarm_right";
    public static final String BODYPART_HAND_RIGHT_TAG        = "hand_right";
    public static final String BODYPART_LOWERLEG_LEFT_TAG     = "lowerleg_left";
    public static final String BODYPART_UPPERLEG_LEFT_TAG     = "upperleg_left";
    public static final String BODYPART_FOOT_LEFT_TAG         = "foot_left";
    public static final String BODYPART_LOWERLEG_RIGHT_TAG    = "lowerleg_right";
    public static final String BODYPART_UPPERLEG_RIGHT_TAG    = "upperleg_right";
    public static final String BODYPART_FOOT_RIGHT_TAG        = "foot_right";
    public static final String BODYPART_UPPERBODY_TAG         = "upperbody";
    public static final String BODYPART_LOWERBODY_TAG         = "lowerbody";
    public static final String BODYPART_KATANA_TAG            = "katana";
    public static final String BODYPART_UNTAGGED_TAG          = "untagged";
    public static final String BODYPART_ALL_TAG               = "all";

    /*Additional Body Parts*/
    public static final String BODY_UNTAGGED_TAG       = "untagged";
    public static final String BODY_KATANA_TAG = "katana";

    /*Messages*/
    public static final String MESSAGE_PLAYER_TAG      = "player";
    public static final String MESSAGE_BODYPART_TAG    = "bodypart";
    public static final String MESSAGE_SENSOR_TYPE_TAG = "sensortype";
    public static final String MESSAGE_VALUE_TAG       = "value";
    public static final String MESSAGE_VALUE_RESET_TAG = "reset";

    public static final String SENSORTYPE_ORIENTATION_ABS_TAG = "orientation_abs";
    public static final String SENSORTYPE_ACCELERATION_REL_TAG = "acceleration_rel";
    public static final String SENSORTYPE_GLOVE_TAG = "glove";

    /*Actions*/
    public static final String ACTION_TYPE_NONE_TAG                 = "none";
    public static final String ACTION_TYPE_HAPTIC_TAG               = "haptic";
    public static final String ACTION_TYPE_SETPLAYER_TAG            = "set_player";
    public static final String ACTION_TYPE_SETBODYPART_TAG          = "set_bodypart";
    public static final String ACTION_TYPE_ENABLESENSOR_TAG         = "enable_sensor";
    public static final String ACTION_TYPE_SETWIFI_TAG              = "set_wifi";


    public static final String ACTION_PLAYER_TAG            = "player";
    public static final String ACTION_BODYPART_TAG          = "bodypart";
    public static final String ACTION_TYPE_TAG              = "type";
    public static final String ACTION_HAPTIC_DURATIONMS_TAG = "duration_ms";
    public static final String ACTION_HAPTIC_STRENGTH_TAG   = "strength";
    public static final String ACTION_SETPLAYER_NEWPLAYER_TAG       = "new_player";
    public static final String ACTION_SETBODYPART_NEWBODYPART_TAG   = "new_bodypart";
    public static final String ACTION_ENABLESENSOR_SENSORTYPE_TAG   = "sensortype";
    public static final String ACTION_ENABLESENSOR_ENABLE_TAG       = "enable";
    public static final String ACTION_SETWIFI_SSID_TAG              = "ssid";
    public static final String ACTION_SETWIFI_PASSWORD_TAG          = "password";
    public static final String ACTION_SETWIFI_MULTICAST_GROUP_TAG   = "multicast_group";


    public final static int COMMUNICATION_STATE_DISCONNECTED = 0;
    public final static int COMMUNICATION_STATE_WAITING_ACK = 1;
    public final static int COMMUNICATION_STATE_CONNECTED = 2;

    public static final int COMMUNICATION_TYPE_WIFI = 0;
    public static final int COMMUNICATION_TYPE_BLUETOOTH = 1;

    public static final int[] REORIENT_IO_AXIS = new int[]{ 1, 2, 3, 0 };
    public static final int[] REORIENT_IO_SIGN = new int[]{ -1, 1, -1, -1 };

    public static final int AR_OUT_AXIS_X = 0;
    public static final int AR_OUT_AXIS_Y = 1;
    public static final int AR_OUT_AXIS_Z = 2;

    public static final int WIFI_PERMISSION_CODE                = 10000;
    public static final int FOREGROUND_SERVICE_PERMISSION_CODE  = 10001;
    public static final int SENSOR_SERVICE_NOTIFICATION_ID      = 10002;
    public static final int BLUETOOTH_PERMISSION_CODE           = 10003;

    public final static float BIG_ORIENTATION_ABS_DIFF = 0.002f;
    public final static float BIG_ACCELERATION_REL_DIFF = 0.05f;

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

    public static final String PLAYER_NAME_DEFAULT = "playerone";
    public static final String MULTICAST_GROUP_DEFAULT = "BN";


    public static final int SENSOR_READ_INTERVAL_MS = 30;

    public static final int CONNECTION_ACK_INTERVAL_MS = 1000;
    public static final int CONNECTION_KEEP_ALIVE_SEND_INTERVAL_MS = 30000;
    public static final int CONNECTION_KEEP_ALIVE_REC_INTERVAL_MS = 60000;
    public static final int MULTICAST_KEEP_ALIVE_REC_INTERVAL_MS = 30000;


}
