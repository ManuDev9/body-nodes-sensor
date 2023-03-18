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

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.util.Log;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.util.Locale;

/**
 * Created by m.bottini on 13/02/2016.
 */
public class BodynodesUtils {

    public final static String TAG = "BodyNodesUtils";

    private final static BluetoothAdapter sBluetoothAdapter;

    static {
        sBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    /**
     *     If the adapter is null, then Bluetooth is not supported
     *     @return true if bluetooth is supported, false otherwise
     */
    public static boolean isBluetoothSupported(){
        return sBluetoothAdapter != null;
    }

    /**
     *
     * @param address it is the mac address of the bluetooth device
     * @return it is the objected representing the bluetooth device
     */
    public static BluetoothDevice getBluetoothDevide(String address){
        return sBluetoothAdapter.getRemoteDevice(address);
    }

    /**
     * @return true if bluetooth is enabled, false otherwise
     */
    public static boolean isBluetoothEnabled(){
        if (sBluetoothAdapter == null) {
            return false;
        }
        return sBluetoothAdapter.isEnabled();
    }

    /**
     *
     * @return the bluetooth adapter
     */
    public static BluetoothAdapter getsBluetoothAdapter(){
        return sBluetoothAdapter;
    }

    public static byte [] float2ByteArray (float value) {
        return ByteBuffer.allocate(4).putFloat(value).array();
    }

    public static String formatIP(int ip) {
        return String.format(
                Locale.ENGLISH,
                "%d.%d.%d.%d",
                (ip & 0xff),
                (ip >> 8 & 0xff),
                (ip >> 16 & 0xff),
                (ip >> 24 & 0xff)
        );
    }

    public static void realignQuat(float[] orientation_in, float[] orientation_out) {
        orientation_out[BodynodesConstants.OA_OUT_AXIS_W] = BodynodesConstants.OA_MUL_AXIS_W * orientation_in[BodynodesConstants.OA_SENSOR_AXIS_W];
        orientation_out[BodynodesConstants.OA_OUT_AXIS_X] = BodynodesConstants.OA_MUL_AXIS_X *orientation_in[BodynodesConstants.OA_SENSOR_AXIS_X];
        orientation_out[BodynodesConstants.OA_OUT_AXIS_Y] = BodynodesConstants.OA_MUL_AXIS_Y *orientation_in[BodynodesConstants.OA_SENSOR_AXIS_Y];
        orientation_out[BodynodesConstants.OA_OUT_AXIS_Z] = BodynodesConstants.OA_MUL_AXIS_Z *orientation_in[BodynodesConstants.OA_SENSOR_AXIS_Z];
    }

    public static InetAddress getLocalhostIpAddress(Activity activity) throws UnknownHostException {
        WifiManager wifiManager = (WifiManager) activity.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        DhcpInfo dhcp = wifiManager.getDhcpInfo();
        int ip = dhcp.gateway;
        return InetAddress.getByName(BodynodesUtils.formatIP(ip));
    }

    public static InetAddress getInetAddressFromStr(String address_str){
        String[] address_astr = address_str.split("\\.");
        try {
            int ip_value_0 = Integer.parseInt(address_astr[0]);
            int ip_value_1 = Integer.parseInt(address_astr[1]);
            int ip_value_2 = Integer.parseInt(address_astr[2]);
            int ip_value_3 = Integer.parseInt(address_astr[3]);
            return InetAddress.getByAddress(new byte[] {
                    (byte) ip_value_0,
                    (byte) ip_value_1,
                    (byte) ip_value_2,
                    (byte) ip_value_3});
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

}
