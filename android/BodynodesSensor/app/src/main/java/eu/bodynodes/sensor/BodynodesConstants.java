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

package eu.bodynodes.sensor;

/**
 * Created by m.bottini on 04/12/2015.
 */
public class BodynodesConstants {

    public final static String NOTIFICATION_CHANNEL_ID = "eu.bodynodes.sensor";
    public final static String NOTIFICATION_CHANNEL_NAME = "Bodynodes Service";

    public static final String ACTION_UPDATE_UI = "eu.bodynodes.sensor.ACTION_UPDATE_UI";
    public static final String ACTION_SENSOR_GLOVE = "eu.bodynodes.sensor.ACTION_SENSOR_GLOVE";
    public static final String ACTION_RECEIVED = "eu.bodynodes.sensor.ACTION_RECEIVED";
    public static final String KEY_JSON_ACTION = "JSON_ACTION";

    /*Body Parts*/
    public static final String BODY_HEAD_TAG             = "head";
    public static final String BODY_HAND_LEFT_TAG        = "hand_left";
    public static final String BODY_FOREARM_LEFT_TAG     = "forearm_left";
    public static final String BODY_UPPERARM_LEFT_TAG    = "upperarm_left";
    public static final String BODY_BODY_TAG             = "body";
    public static final String BODY_FOREARM_RIGHT_TAG    = "forearm_right";
    public static final String BODY_UPPERARM_RIGHT_TAG   = "upperarm_right";
    public static final String BODY_HAND_RIGHT_TAG       = "hand_right";
    public static final String BODY_LOWERLEG_LEFT_TAG    = "lowerleg_left";
    public static final String BODY_UPPERLEG_LEFT_TAG    = "upperleg_left";
    public static final String BODY_FOOT_LEFT_TAG        = "shoe_left";
    public static final String BODY_LOWERLEG_RIGHT_TAG   = "lowerleg_right";
    public static final String BODY_UPPERLEG_RIGHT_TAG   = "upperleg_right";
    public static final String BODY_FOOT_RIGHT_TAG       = "shoe_right";

    /*Additional Body Parts*/
    public static final String BODY_UNTAGGED_TAG       = "untagged";
    public static final String BODY_KATANA_TAG = "katana";

    /*Messages*/
    public static final String MESSAGE_PLAYER_TAG      = "player";
    public static final String MESSAGE_BODYPART_TAG    = "bodypart";
    public static final String MESSAGE_SENSOR_TYPE_TAG = "sensortype";
    public static final String MESSAGE_VALUE_TAG       = "value";

    public static final String SENSORTYPE_ORIENTATION_ABS_TAG = "orientation_abs";
    public static final String SENSORTYPE_ACCELERATION_REL_TAG = "acceleration_rel";
    public static final String SENSORTYPE_GLOVE_TAG = "glove";
    public static final String PLAYER_NAME_DEFAULT = "playerone";

    /*Actions*/

    public static final String ACTION_TYPE_TAG              = "type";
    public static final String ACTION_HAPTIC_TAG            = "haptic";
    public static final String ACTION_HAPTIC_DURATIONMS_TAG = "duration_ms";
    public static final String ACTION_HAPTIC_STRENGTH_TAG   = "strength";

    public static final int ACTION_CODE_HAPTIC            = 0;
    public static final int ACTION_CODE_SETPLAYER         = 1;
    public static final int ACTION_CODE_SETBODYPART       = 2;

    public static final String[] BODY_PART_TAGS = {
                BODY_HEAD_TAG,
                BODY_HAND_LEFT_TAG,
                BODY_FOREARM_LEFT_TAG,
                BODY_UPPERARM_LEFT_TAG,
                BODY_BODY_TAG,
                BODY_FOREARM_RIGHT_TAG,
                BODY_UPPERARM_RIGHT_TAG,
                BODY_HAND_RIGHT_TAG,
                BODY_LOWERLEG_LEFT_TAG,
                BODY_UPPERLEG_LEFT_TAG,
                BODY_FOOT_LEFT_TAG,
                BODY_LOWERLEG_RIGHT_TAG,
                BODY_UPPERLEG_RIGHT_TAG,
                BODY_FOOT_RIGHT_TAG,
                BODY_KATANA_TAG,
                BODY_UNTAGGED_TAG
            };

    public final static int COMMUNICATION_STATE_DISCONNECTED = 0;
    public final static int COMMUNICATION_STATE_WAITING_ACK = 1;
    public final static int COMMUNICATION_STATE_CONNECTED = 2;

    public static final int COMMUNICATION_TYPE_WIFI = 0;
    public static final int COMMUNICATION_TYPE_BLUETOOTH = 1;

    // Device Specific Axis Configuration
    public static final int OA_OUT_AXIS_W = 0;
    public static final int OA_OUT_AXIS_X = 1;
    public static final int OA_OUT_AXIS_Y = 2;
    public static final int OA_OUT_AXIS_Z = 3;

    public static final int OA_SENSOR_AXIS_W = OA_OUT_AXIS_W;
    public static final int OA_SENSOR_AXIS_X = OA_OUT_AXIS_Z;
    public static final int OA_SENSOR_AXIS_Y = OA_OUT_AXIS_Y;
    public static final int OA_SENSOR_AXIS_Z = OA_OUT_AXIS_X;

    public static final int OA_MUL_AXIS_W = -1;
    public static final int OA_MUL_AXIS_X = 1;
    public static final int OA_MUL_AXIS_Y = 1;
    public static final int OA_MUL_AXIS_Z = -1;

    public static final int AR_OUT_AXIS_X = 0;
    public static final int AR_OUT_AXIS_Y = 1;
    public static final int AR_OUT_AXIS_Z = 2;

    public static final int WIFI_PERMISSION_CODE                = 10000;
    public static final int FOREGROUND_SERVICE_PERMISSION_CODE  = 10001;
    public static final int SENSOR_SERVICE_NOTIFICATION_ID      = 10002;
    public static final int BLUETOOTH_PERMISSION_CODE            = 10003;

    public final static float BIG_ORIENTATION_ABS_DIFF = 0.002f;
    public final static float BIG_ACCELERATION_REL_DIFF = 0.05f;

    // SHARED REFS TAGS
    public static final String BODYNODES_SHARED_PREFS = "BODYNODES_SHARED_PREFS";
    public static final String LOCAL_PORT_IN_NUMBER = "LOCAL_PORT_IN_NUMBER";
    public static final String REMOTE_PORT_OUT_NUMBER = "REMOTE_PORT_OUT_NUMBER";
    public static final String COMMUNICATION_TYPE = "COMMUNICATION_TYPE";
    public static final String BODYPART = "BODYPART";
    public static final String GLOVE_BODYPART = "GLOVE_BODYPART";
    public static final String GLOVE_DATA = "GLOVE_DATA";
    public static final String ORIENTATION_ABSOLUTE_ENABLED = "ORIENTATION_ABSOLUTE_ENABLED";
    public static final String ACCELERATION_RELATIVE_ENABLED = "ACCELERATION_RELATIVE_ENABLED";
    public static final String SENSOR_INTERVAL_MS = "SENSOR_INTERVAL_MS";
    public static final String PLAYER_NAME = "PLAYER_NAME";

    public static final int REMOTE_PORT_OUT_NUMBER_DEFAULT = 8000;
    public static final int LOCAL_PORT_IN_NUMBER_DEFAULT = 9000;
    public static final int SENSOR_INTERVAL_MS_DEFAULT = 30;
}
