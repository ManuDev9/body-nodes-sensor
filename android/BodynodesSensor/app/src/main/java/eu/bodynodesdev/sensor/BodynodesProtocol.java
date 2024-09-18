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

import org.json.JSONException;
import org.json.JSONObject;

public class BodynodesProtocol {

    public final static String TAG = "BodynodesProtocol";

    // Specification Version
    public final static String VERSION = "d1.0";

    public static JSONObject makeMessageWifi(String player, String bodypart, String sensortype, int[] values){
        JSONObject jsonObject = new JSONObject();

        try {
            jsonObject.put(BnConstants.MESSAGE_PLAYER_TAG, player);
            jsonObject.put(BnConstants.MESSAGE_BODYPART_TAG, bodypart);
            jsonObject.put(BnConstants.MESSAGE_SENSOR_TYPE_TAG, sensortype);
            if(sensortype.equals(BnConstants.SENSORTYPE_GLOVE_TAG)){
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[0]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[1]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[2]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[3]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[4]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[5]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[6]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[7]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[8]);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return jsonObject;
    }

    public static JSONObject makeMessageWifi(String player, String bodypart, String sensortype, float[] values){
        JSONObject jsonObject = new JSONObject();

        try {
            jsonObject.put(BnConstants.MESSAGE_PLAYER_TAG, player);
            jsonObject.put(BnConstants.MESSAGE_BODYPART_TAG, bodypart);
            jsonObject.put(BnConstants.MESSAGE_SENSOR_TYPE_TAG, sensortype);
            if(sensortype.equals(BnConstants.SENSORTYPE_ORIENTATION_ABS_TAG)){
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[0]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[1]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[2]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[3]);
            } else if(sensortype.equals(BnConstants.SENSORTYPE_ACCELERATION_REL_TAG)) {
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[0]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[1]);
                jsonObject.accumulate(BnConstants.MESSAGE_VALUE_TAG,values[2]);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return jsonObject;
    }

    public static JSONObject makeMessageWifi(String player, String bodypart, String sensortype, String value){
        JSONObject jsonObject = new JSONObject();

        try {
            jsonObject.put(BnConstants.MESSAGE_PLAYER_TAG, player);
            jsonObject.put(BnConstants.MESSAGE_BODYPART_TAG, bodypart);
            jsonObject.put(BnConstants.MESSAGE_SENSOR_TYPE_TAG, sensortype);
            if(sensortype.equals(BnConstants.SENSORTYPE_ORIENTATION_ABS_TAG)){
                jsonObject.put(BnConstants.MESSAGE_VALUE_TAG,value);
            } else if(sensortype.equals(BnConstants.SENSORTYPE_ACCELERATION_REL_TAG)) {
                jsonObject.put(BnConstants.MESSAGE_VALUE_TAG,value);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return jsonObject;
    }

}
