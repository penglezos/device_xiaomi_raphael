/*
 * Copyright (C) 2015 The CyanogenMod Project
 *               2017-2018 The LineageOS Project
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

package org.lineageos.settings.doze;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.SystemClock;
import android.util.Log;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class PickupSensor implements SensorEventListener {
    private static final boolean DEBUG = false;
    private static final String TAG = "PickupSensor";

    private static final int MIN_PULSE_INTERVAL_MS = 2500;
    private static final int MIN_WAKEUP_INTERVAL_MS = 1000;
    private static final int WAKELOCK_TIMEOUT_MS = 300;

    private PowerManager mPowerManager;
    private SensorManager mSensorManager;
    private Sensor mSensor;
    private WakeLock mWakeLock;
    private Context mContext;
    private ExecutorService mExecutorService;

    private long mEntryTimestamp;

    public PickupSensor(Context context) {
        mContext = context;
        mPowerManager = mContext.getSystemService(PowerManager.class);
        mSensorManager = mContext.getSystemService(SensorManager.class);
        mSensor = DozeUtils.getSensor(mSensorManager, "xiaomi.sensor.pickup");
        mWakeLock = mPowerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);
        mExecutorService = Executors.newSingleThreadExecutor();
    }

    private Future<?> submit(Runnable runnable) { return mExecutorService.submit(runnable); }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (DEBUG)
            Log.d(TAG, "Got sensor event: " + event.values[0]);

        long delta = SystemClock.elapsedRealtime() - mEntryTimestamp;
        if (delta < MIN_PULSE_INTERVAL_MS) {
            return;
        }

        mEntryTimestamp = SystemClock.elapsedRealtime();

        if (event.values[0] == 1) {
            if (DozeUtils.isPickUpSetToWake(mContext)) {
                mWakeLock.acquire(WAKELOCK_TIMEOUT_MS);
                mPowerManager.wakeUpWithProximityCheck(SystemClock.uptimeMillis(),
                        PowerManager.WAKE_REASON_GESTURE, TAG);
            } else {
                DozeUtils.launchDozePulse(mContext);
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        /* Empty */
    }

    protected void enable() {
        if (DEBUG)
            Log.d(TAG, "Enabling");
        submit(() -> {
            mSensorManager.registerListener(this, mSensor, SensorManager.SENSOR_DELAY_NORMAL);
            mEntryTimestamp = SystemClock.elapsedRealtime();
        });
    }

    protected void disable() {
        if (DEBUG)
            Log.d(TAG, "Disabling");
        submit(() -> { mSensorManager.unregisterListener(this, mSensor); });
    }
}
