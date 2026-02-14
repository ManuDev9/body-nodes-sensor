package eu.bodynodesdev.sensor;

import android.content.Context;
import android.content.Intent;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.rule.ServiceTestRule;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.util.concurrent.TimeoutException;

import eu.bodynodesdev.sensor.data.AppData;
import eu.bodynodesdev.sensor.services.SensorServiceBLE;
import eu.bodynodesdev.sensor.services.SensorServiceBluetooth;
import eu.bodynodesdev.sensor.services.SensorServiceWifi;


@RunWith(AndroidJUnit4.class)
public class TestAllServices {

    @Rule
    public final ServiceTestRule serviceRule = new ServiceTestRule();

    @Test
    public void testSensorServiceBLE_basicStartService() throws TimeoutException {

        Context context = androidx.test.platform.app.InstrumentationRegistry.getInstrumentation().getTargetContext();

        AppData.setCommunitcationType(context, BnAppConstants.COMMUNICATION_TYPE_BLE);
        // Launch the service
        Intent intent = new Intent(
                context,
                SensorServiceBLE.class
        );
        serviceRule.startService(intent);
    }

    @Test
    public void testSensorServiceBluetooth_basicStartService() throws TimeoutException {

        Context context = androidx.test.platform.app.InstrumentationRegistry.getInstrumentation().getTargetContext();

        AppData.setCommunitcationType(context, BnAppConstants.COMMUNICATION_TYPE_BLUETOOTH);
        // Launch the service
        Intent intent = new Intent(
                context,
                SensorServiceBluetooth.class
        );
        serviceRule.startService(intent);
    }

    @Test
    public void testSensorServiceWifi_basicStartService() throws TimeoutException {

        Context context = androidx.test.platform.app.InstrumentationRegistry.getInstrumentation().getTargetContext();

        AppData.setCommunitcationType(context, BnAppConstants.COMMUNICATION_TYPE_WIFI);
        // Launch the service
        Intent intent = new Intent(
                context,
                SensorServiceWifi.class
        );
        serviceRule.startService(intent);
    }

}
