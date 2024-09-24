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

package eu.bodynodesdev.sensor.data;

import android.content.Context;
import android.content.SharedPreferences;

import eu.bodynodesdev.common.BnConstants;
import eu.bodynodesdev.sensor.BnAppConstants;

public class BnSensorAppData {

    public static String getPlayerName(Context context){
        SharedPreferences sharedPref = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getString(BnAppConstants.PLAYER_NAME, BnConstants.PLAYER_NAME_DEFAULT);
    }

    public static void setPlayerName(Context context, String player) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putString(BnAppConstants.PLAYER_NAME, player);
        editor.apply();
    }

    public static String getBodypart(Context context){
        SharedPreferences sharedPref = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getString(BnAppConstants.BODYPART, BnConstants.BODY_KATANA_TAG);
    }

    public static void setBodypart(Context context, String bodypart) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putString(BnAppConstants.BODYPART, bodypart);
        editor.apply();
    }

    public static String getGloveBodypart(Context context){
        SharedPreferences sharedPref = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getString(BnAppConstants.GLOVE_BODYPART, BnConstants.BODYPART_HAND_LEFT_TAG);
    }

    public static void setGloveBodypart(Context context, String bodypart) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putString(BnAppConstants.GLOVE_BODYPART, bodypart);
        editor.apply();
    }

    public static boolean isOrientationAbsSensorEnabled(Context context) {
        SharedPreferences sharedPref = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getBoolean(BnAppConstants.ORIENTATION_ABSOLUTE_ENABLED, false);
    }

    public static void setOrientationAbsSensorEnabled(Context context, boolean orientationAbsSensorEnabled) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putBoolean(BnAppConstants.ORIENTATION_ABSOLUTE_ENABLED, orientationAbsSensorEnabled);
        editor.apply();
    }

    public static boolean isAccelerationRelSensorEnabled(Context context) {
        SharedPreferences sharedPref = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getBoolean(BnAppConstants.ACCELERATION_RELATIVE_ENABLED, false);
    }

    public static void setAccelerationRelSensorEnabled(Context context, boolean accelerationRelSensorEnabled) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putBoolean(BnAppConstants.ACCELERATION_RELATIVE_ENABLED, accelerationRelSensorEnabled);
        editor.apply();
    }

    public static String getMulticastGroup(Context context){
        SharedPreferences sharedPref = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE);
        return sharedPref.getString(BnAppConstants.MULTICAST_GROUP, BnConstants.MULTICAST_GROUP_DEFAULT);
    }

    public static void setMulticastGroup(Context context, String player) {
        SharedPreferences.Editor editor = context.getSharedPreferences(BnAppConstants.BODYNODES_SHARED_PREFS, Context.MODE_PRIVATE).edit();
        editor.putString(BnAppConstants.MULTICAST_GROUP, player);
        editor.apply();
    }


}
