package eu.bodynodesdev.sensor;


import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.core.app.ActivityScenario;

import org.junit.Test;
import org.junit.runner.RunWith;

import eu.bodynodesdev.sensor.pages.MainSensorActivity;
import eu.bodynodesdev.sensor.pages.SettingsActivity;

@RunWith(AndroidJUnit4.class)
public class TestAllActivities {

    @Test
    public void testMainSensorActivity_launchesSuccessfully() {
        ActivityScenario.launch(MainSensorActivity.class);
    }

    @Test
    public void testSettingsActivity_launchesSuccessfully() {
        ActivityScenario.launch(SettingsActivity.class);
    }
}
