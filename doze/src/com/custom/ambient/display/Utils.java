/*
 * Copyright (c) 2015 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.custom.ambient.display;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.PowerManager;
import android.os.SystemClock;
import android.os.UserHandle;
import android.provider.Settings;
import android.util.Log;

import androidx.preference.PreferenceManager;

public final class Utils {

    private static final String TAG = "DozeUtils";
    private static final boolean DEBUG = false;

    private static final String DOZE_INTENT = "com.android.systemui.doze.pulse";

    protected static final String AMBIENT_DISPLAY_KEY = "ambient_display";
    protected static final String PICK_UP_KEY = "pick_up";
    protected static final String GESTURE_HAND_WAVE_KEY = "gesture_hand_wave";
    protected static final String GESTURE_POCKET_KEY = "gesture_pocket";
    protected static final String WAKE_ON_GESTURE_KEY = "wake_on_gesture";

    protected static void startService(Context context) {
        if (DEBUG) Log.d(TAG, "Starting service");
        if (!isServiceRunning(DozeService.class, context)) {
            context.startService(new Intent(context, DozeService.class));
        }
    }

    private static boolean isServiceRunning(Class<?> serviceClass, Context context) {
        ActivityManager manager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        if (manager != null) {
            for (RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
                if (serviceClass.getName().equals(service.service.getClassName())) {
                    return true;
                }
            }
        }
        return false;
    }

    protected static void stopService(Context context) {
        if (DEBUG) Log.d(TAG, "Stopping service");
        context.stopService(new Intent(context, DozeService.class));
    }

    protected static boolean isDozeEnabled(Context context) {
        return Settings.Secure.getInt(context.getContentResolver(),
                Settings.Secure.DOZE_ENABLED, 1) != 0;
    }

    protected static boolean pickUpEnabled(Context context) {
        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(PICK_UP_KEY, false);
    }

    protected static boolean handwaveGestureEnabled(Context context) {
        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(GESTURE_HAND_WAVE_KEY, false);
    }

    protected static boolean pocketGestureEnabled(Context context) {
        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(GESTURE_POCKET_KEY, false);
    }

    protected static boolean isWakeOnGestureEnabled(Context context) {
        return PreferenceManager.getDefaultSharedPreferences(context)
                .getBoolean(WAKE_ON_GESTURE_KEY, false);
    }

    protected static boolean enableDoze(boolean enable, Context context) {
        boolean enabled = Settings.Secure.putInt(context.getContentResolver(),
                Settings.Secure.DOZE_ENABLED, enable ? 1 : 0);
        // don't start the service, for notifications pulse we don't need the proximity sensor check here
        return enabled;
    }

    protected static boolean enablePickUp(boolean enable, Context context) {
        // shared pref value already updated by DozeSettings.onPreferenceChange
        manageService(context);
        return enable;
    }

    protected static boolean enableHandWave(boolean enable, Context context) {
        // shared pref value already updated by DozeSettings.onPreferenceChange
        manageService(context);
        return enable;
    }

    protected static boolean enablePocketMode(boolean enable, Context context) {
        // shared pref value already updated by DozeSettings.onPreferenceChange
        manageService(context);
        return enable;
    }

    protected static boolean wakeOnGesture(boolean enable, Context context) {
        // shared pref value already updated by DozeSettings.onPreferenceChange
        manageService(context);
        return enable;
    }

    private static void manageService(Context context) {
        if (sensorsEnabled(context)) {
            startService(context);
        } else {
            stopService(context);
        }
    }

    protected static void wakeOrLaunchDozePulse(Context context) {
        if (isWakeOnGestureEnabled(context)) {
            if (DEBUG) Log.d(TAG, "Wake up display");
            PowerManager powerManager = context.getSystemService(PowerManager.class);
            powerManager.wakeUp(SystemClock.uptimeMillis(), PowerManager.WAKE_REASON_GESTURE, TAG);
        } else {
            if (DEBUG) Log.d(TAG, "Launch doze pulse");
            context.sendBroadcastAsUser(
                    new Intent(DOZE_INTENT), new UserHandle(UserHandle.USER_CURRENT));
        }
    }

    protected static boolean sensorsEnabled(Context context) {
        return pickUpEnabled(context) || handwaveGestureEnabled(context)
                || pocketGestureEnabled(context);
    }

    protected static Sensor getSensor(SensorManager sm, String type) {
        for (Sensor sensor : sm.getSensorList(Sensor.TYPE_ALL)) {
            if (type.equals(sensor.getStringType())) {
                return sensor;
            }
        }
        return null;
    }
}
