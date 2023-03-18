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

package eu.bodynodes.sensor;

import com.illposed.osc.OSCMessage;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class BodynodesProtocol {

    public final static String TAG = "BodynodesProtocol";

    // Specification Version
    public final static String VERSION = "d1.0";

    public static JSONObject makeMessageWifi(String player, String bodypart, String sensortype, float[] values){
        JSONObject jsonObject = new JSONObject();

        try {
            jsonObject.put(BodynodesConstants.MESSAGE_PLAYER_TAG, player);
            jsonObject.put(BodynodesConstants.MESSAGE_BODYPART_TAG, bodypart);
            jsonObject.put(BodynodesConstants.MESSAGE_SENSOR_TYPE_TAG, sensortype);
            if(sensortype.equals(BodynodesConstants.SENSORTYPE_ORIENTATION_ABS_TAG)){
                jsonObject.accumulate(BodynodesConstants.MESSAGE_VALUE_TAG,values[0]);
                jsonObject.accumulate(BodynodesConstants.MESSAGE_VALUE_TAG,values[1]);
                jsonObject.accumulate(BodynodesConstants.MESSAGE_VALUE_TAG,values[2]);
                jsonObject.accumulate(BodynodesConstants.MESSAGE_VALUE_TAG,values[3]);
            } else if(sensortype.equals(BodynodesConstants.SENSORTYPE_ACCELERATION_REL_TAG)) {
                jsonObject.accumulate(BodynodesConstants.MESSAGE_VALUE_TAG,values[0]);
                jsonObject.accumulate(BodynodesConstants.MESSAGE_VALUE_TAG,values[1]);
                jsonObject.accumulate(BodynodesConstants.MESSAGE_VALUE_TAG,values[2]);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return jsonObject;
    }

    public static int parseActionWifi(JSONObject jsonAction){
        try {
            if(jsonAction.has(BodynodesConstants.ACTION_ACTION_TAG)) {
                if (BodynodesConstants.ACTION_HAPTIC_TAG.equals(jsonAction.getString(BodynodesConstants.ACTION_ACTION_TAG))) {
                    if (jsonAction.has(BodynodesConstants.ACTION_HAPTIC_STRENGTH_TAG) && jsonAction.has(BodynodesConstants.ACTION_HAPTIC_DURATIONMS_TAG)) {
                        return BodynodesConstants.ACTION_CODE_HAPTIC;
                    }
                }
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return -1;
    }

    public static String createOscPath(String player, String bodypart, String sensortype) {
        return "/obBn";
    }

    public static OSCMessage makeMessageWifiOsc(String player, String bodypart, String sensortype, float[] values) {
        final List<Object> args = new ArrayList<>(0);
        if(sensortype.equals(BodynodesConstants.SENSORTYPE_ORIENTATION_ABS_TAG)){
            args.add(player);
            args.add(bodypart);
            args.add(sensortype);

            args.add(values[BodynodesConstants.OA_OUT_AXIS_W]);
            args.add(values[BodynodesConstants.OA_OUT_AXIS_X]);
            args.add(values[BodynodesConstants.OA_OUT_AXIS_Y]);
            args.add(values[BodynodesConstants.OA_OUT_AXIS_Z]);
        } else if(sensortype.equals(BodynodesConstants.SENSORTYPE_ACCELERATION_REL_TAG)){
            args.add(player);
            args.add(bodypart);
            args.add(sensortype);

            args.add(values[BodynodesConstants.AR_OUT_AXIS_X]);
            args.add(values[BodynodesConstants.AR_OUT_AXIS_Y]);
            args.add(values[BodynodesConstants.AR_OUT_AXIS_Z]);
        }
        return new OSCMessage(createOscPath(player, bodypart, sensortype), args);
    }
}
