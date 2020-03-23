/*
 * Copyright (C) 2019 The LineageOS Project
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

package org.lineageos.settings.popupcamera;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.AudioAttributes;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Handler;
import android.os.IBinder;
import android.os.UserHandle;
import android.util.Log;

import vendor.xiaomi.hardware.motor.V1_0.IMotor;

public class PopupCameraService extends Service {

    private static final String TAG = "PopupCameraService";
    private static final boolean DEBUG = false;
    private static final String closeCameraState = "0";
    private static final String openCameraState = "1";
    private static final int FREE_FALL_SENSOR_ID = 33171042;
    private static final String GREEN_LED_PATH = "/sys/class/leds/green/brightness";
    private static final String BLUE_LED_PATH = "/sys/class/leds/blue/brightness";
    private static String mCameraState = "-1";
    private static Handler mHandler = new Handler();
    private IMotor mMotor = null;
    private boolean mMotorBusy = false;
    private SensorManager mSensorManager;
    private Sensor mFreeFallSensor;
    private PopupCameraPreferences mPopupCameraPreferences;
    private String[] mSoundNames = { "popup_muqin_up.ogg", "popup_muqin_down.ogg", "popup_yingyan_up.ogg",
            "popup_yingyan_down.ogg", "popup_mofa_up.ogg", "popup_mofa_down.ogg", "popup_jijia_up.ogg",
            "popup_jijia_down.ogg", "popup_chilun_up.ogg", "popup_chilun_down.ogg", "popup_cangmen_up.ogg",
            "popup_cangmen_down.ogg" };
    private SoundPool mSoundPool;
    private int[] mSounds = new int[mSoundNames.length];
    private SensorEventListener mFreeFallListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            if (event.sensor.getType() == FREE_FALL_SENSOR_ID && event.values[0] == 2.0f) {
                updateMotor(closeCameraState);
                goBackHome();
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
        }
    };
    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (android.content.Intent.ACTION_CAMERA_STATUS_CHANGED.equals(action)) {
                mCameraState = intent.getExtras().getString(android.content.Intent.EXTRA_CAMERA_STATE);
                updateMotor(mCameraState);
            }
        }
    };

    @Override
    public void onCreate() {
        mSensorManager = getSystemService(SensorManager.class);
        mFreeFallSensor = mSensorManager.getDefaultSensor(FREE_FALL_SENSOR_ID);
        registerReceiver();
        mPopupCameraPreferences = new PopupCameraPreferences(this);
        mSoundPool = new SoundPool.Builder().setMaxStreams(1)
                .setAudioAttributes(
                        new AudioAttributes.Builder().setUsage(AudioAttributes.USAGE_ASSISTANCE_SONIFICATION)
                                .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                                .setFlags(AudioAttributes.FLAG_AUDIBILITY_ENFORCED).build())
                .build();
        int i = 0;
        for (String soundName : mSoundNames) {
            mSounds[i] = mSoundPool.load("/system/media/audio/ui/" + soundName, 1);
            i++;
        }

        try {
            mMotor = IMotor.getService();
        } catch (Exception e) {
            // Do nothing
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (DEBUG)
            Log.d(TAG, "Starting service");
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        if (DEBUG)
            Log.d(TAG, "Destroying service");
        unregisterReceiver(mIntentReceiver);
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void registerReceiver() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        filter.addAction(android.content.Intent.ACTION_CAMERA_STATUS_CHANGED);
        registerReceiver(mIntentReceiver, filter);
    }

    private void updateMotor(String cameraState) {
        final Runnable r = new Runnable() {
            @Override
            public void run() {
                mMotorBusy = true;
                mHandler.postDelayed(() -> {
                    mMotorBusy = false;
                }, 1200);
                if (mMotor == null)
                    return;
                try {
                    if (cameraState.equals(openCameraState) && mMotor.getMotorStatus() == 13) {
                        lightUp();
                        playSoundEffect(openCameraState);
                        mMotor.popupMotor(1);
                        mSensorManager.registerListener(mFreeFallListener, mFreeFallSensor,
                                SensorManager.SENSOR_DELAY_NORMAL);
                    } else if (cameraState.equals(closeCameraState) && mMotor.getMotorStatus() == 11) {
                        lightUp();
                        playSoundEffect(closeCameraState);
                        mMotor.takebackMotor(1);
                        mSensorManager.unregisterListener(mFreeFallListener, mFreeFallSensor);
                    }
                } catch (Exception e) {
                    // Do nothing
                }
            }
        };
        if (mMotorBusy) {
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (mMotorBusy) {
                        mHandler.postDelayed(this, 100);
                    } else {
                        mHandler.post(r);
                    }
                }
            }, 100);
        } else {
            mHandler.post(r);
        }
    }

    private void lightUp() {
        if (mPopupCameraPreferences.isLedAllowed()) {
            FileUtils.writeLine(GREEN_LED_PATH, "255");
            FileUtils.writeLine(BLUE_LED_PATH, "255");

            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    FileUtils.writeLine(GREEN_LED_PATH, "0");
                    FileUtils.writeLine(BLUE_LED_PATH, "0");
                }
            }, 1200);
        }
    }

    private void playSoundEffect(String state) {
        AudioManager audioManager = (AudioManager) getApplicationContext().getSystemService(Context.AUDIO_SERVICE);
        if (audioManager.getRingerMode() != AudioManager.RINGER_MODE_NORMAL) {
            return;
        }
        int soundEffect = Integer.parseInt(mPopupCameraPreferences.getSoundEffect());
        if (soundEffect != -1) {
            if (state.equals(closeCameraState)) {
                soundEffect++;
            }
            mSoundPool.play(mSounds[soundEffect], 1.0f, 1.0f, 0, 0, 1.0f);
        }
    }

    public void goBackHome() {
        Intent homeIntent = new Intent(Intent.ACTION_MAIN);
        homeIntent.addCategory(Intent.CATEGORY_HOME);
        homeIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivityAsUser(homeIntent, null, UserHandle.CURRENT);
    }
}
