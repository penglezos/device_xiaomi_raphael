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

import android.annotation.NonNull;
import android.app.AlertDialog;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.hardware.camera2.CameraManager;
import android.media.AudioAttributes;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemClock;
import android.os.UserHandle;
import android.provider.Settings;
import android.util.Log;
import android.view.WindowManager;
import org.lineageos.settings.R;
import org.lineageos.settings.utils.FileUtils;
import vendor.xiaomi.hardware.motor.V1_0.IMotor;
import vendor.xiaomi.hardware.motor.V1_0.IMotorCallback;
import vendor.xiaomi.hardware.motor.V1_0.MotorEvent;

public class PopupCameraService extends Service implements Handler.Callback {
    private static final String TAG = "PopupCameraService";
    private static final boolean DEBUG = false;
    private static final String alwaysOnDialogKey = "always_on_camera_dialog";

    private int[] mSounds;
    private boolean mMotorBusy = false;
    private long mClosedEvent;
    private long mOpenEvent;
    private boolean mScreenOn = true;
    private int mDialogThemeResID;

    private AlertDialog mAlertDialog;
    private Handler mHandler = new Handler(this);
    private IMotor mMotor = null;
    private IMotorCallback mMotorStatusCallback;
    private boolean mMotorCalibrating = false;
    private boolean mErrorDialogShowing;
    private final Object mLock = new Object();
    private SensorManager mSensorManager;
    private Sensor mFreeFallSensor;
    private PopupCameraPreferences mPopupCameraPreferences;
    private SoundPool mSoundPool;

    private boolean mLedBusy = false;
    private boolean mLedBreathing = false;
    private String mLedBrightness = "0";

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(Intent.ACTION_SCREEN_OFF)) {
                mScreenOn = false;
            } else if (action.equals(Intent.ACTION_SCREEN_ON)) {
                mScreenOn = true;
            }
        }
    };

    private CameraManager.AvailabilityCallback availabilityCallback =
            new CameraManager.AvailabilityCallback() {
                @Override
                public void onCameraAvailable(@NonNull String cameraId) {
                    super.onCameraAvailable(cameraId);
                    if (cameraId.equals(Constants.FRONT_CAMERA_ID)) {
                        mClosedEvent = SystemClock.elapsedRealtime();
                        if (SystemClock.elapsedRealtime() - mOpenEvent
                                        < Constants.CAMERA_EVENT_DELAY_TIME
                                && mHandler.hasMessages(Constants.MSG_CAMERA_OPEN)) {
                            mHandler.removeMessages(Constants.MSG_CAMERA_OPEN);
                        }
                        mHandler.sendEmptyMessageDelayed(
                                Constants.MSG_CAMERA_CLOSED, Constants.CAMERA_EVENT_DELAY_TIME);
                    }
                }

                @Override
                public void onCameraUnavailable(@NonNull String cameraId) {
                    super.onCameraAvailable(cameraId);
                    if (cameraId.equals(Constants.FRONT_CAMERA_ID)) {
                        mOpenEvent = SystemClock.elapsedRealtime();
                        if (SystemClock.elapsedRealtime() - mClosedEvent
                                        < Constants.CAMERA_EVENT_DELAY_TIME
                                && mHandler.hasMessages(Constants.MSG_CAMERA_CLOSED)) {
                            mHandler.removeMessages(Constants.MSG_CAMERA_CLOSED);
                        }
                        mHandler.sendEmptyMessageDelayed(
                                Constants.MSG_CAMERA_OPEN, Constants.CAMERA_EVENT_DELAY_TIME);
                    }
                }
            };

    private SensorEventListener mFreeFallListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent event) {
            if (event.values[0] == 2.0f) {
                try {
                    mMotor.takebackMotorShortly();
                    mSensorManager.unregisterListener(mFreeFallListener, mFreeFallSensor);
                } catch (RemoteException e) {
                    // Do nothing
                }
                goBackHome();
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {}
    };

    @Override
    public void onCreate() {
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(Intent.ACTION_SCREEN_OFF);
        intentFilter.addAction(Intent.ACTION_SCREEN_ON);
        registerReceiver(mIntentReceiver, intentFilter);
        CameraManager cameraManager = getSystemService(CameraManager.class);
        cameraManager.registerAvailabilityCallback(availabilityCallback, null);
        mDialogThemeResID = android.R.style.Theme_DeviceDefault_Light_Dialog_Alert;
        mSensorManager = getSystemService(SensorManager.class);
        mFreeFallSensor = mSensorManager.getDefaultSensor(Constants.FREE_FALL_SENSOR_ID);
        mPopupCameraPreferences = new PopupCameraPreferences(this);
        mSoundPool =
                new SoundPool.Builder()
                        .setMaxStreams(1)
                        .setAudioAttributes(
                                new AudioAttributes.Builder()
                                        .setUsage(AudioAttributes.USAGE_ASSISTANCE_SONIFICATION)
                                        .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                                        .setFlags(AudioAttributes.FLAG_AUDIBILITY_ENFORCED)
                                        .build())
                        .build();

        String[] soundNames = getResources().getStringArray(R.array.popupcamera_effects_names);
        mSounds = new int[soundNames.length];
        for (int i = 0; i < soundNames.length; i++) {
            mSounds[i] = mSoundPool.load(Constants.POPUP_SOUND_PATH + soundNames[i], 1);
        }

        try {
            mMotor = IMotor.getService();
            int status = mMotor.getMotorStatus();
            if (status == Constants.MOTOR_STATUS_POPUP_OK
                    || status == Constants.MOTOR_STATUS_TAKEBACK_JAMMED) {
                mMotor.takebackMotor(1);
                Thread.sleep(1200);
            }
            mMotorStatusCallback = new MotorStatusCallback();
            mMotor.setMotorCallback(mMotorStatusCallback);
        } catch (InterruptedException | RemoteException e) {
            // Do nothing
        }
    }

    private final class MotorStatusCallback extends IMotorCallback.Stub {
        public MotorStatusCallback() {}

        @Override
        public void onNotify(MotorEvent event) {
            int status = event.vaalue;
            int cookie = event.cookie;
            if (DEBUG)
                Log.d(TAG, "onNotify: cookie=" + cookie + ", status=" + status);
            synchronized (mLock) {
                if (status == Constants.MOTOR_STATUS_CALIB_OK
                        || status == Constants.MOTOR_STATUS_CALIB_ERROR) {
                    showCalibrationResult(status);
                } else if (status == Constants.MOTOR_STATUS_PRESSED) {
                    updateMotor(Constants.CLOSE_CAMERA_STATE);
                    goBackHome();
                } else if (status == Constants.MOTOR_STATUS_POPUP_JAMMED
                        || status == Constants.MOTOR_STATUS_TAKEBACK_JAMMED) {
                    showErrorDialog();
                }
            }
        }
    }

    protected void calibrateMotor() {
        synchronized (mLock) {
            if (mMotorCalibrating)
                return;
            if (mMotor == null) {
                try {
                    mMotor = IMotor.getService();
                } catch (RemoteException e) {
                    // Do nothing
                }
                if (mMotor == null)
                    return;
            }
            try {
                mMotorCalibrating = true;
                mMotor.calibration();
            } catch (RemoteException e) {
                // Do nothing
            }
            mHandler.postDelayed(() -> mMotorCalibrating = false, 7000);
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

    private void updateMotor(String cameraState) {
        if (mMotor == null) {
            return;
        }
        final Runnable r = () -> {
            mMotorBusy = true;
            try {
                int status = mMotor.getMotorStatus();
                if (DEBUG)
                    Log.d(TAG, "updateMotor: status=" + status + ", cameraState=" + cameraState);
                if (cameraState.equals(Constants.OPEN_CAMERA_STATE)
                        && (status == Constants.MOTOR_STATUS_TAKEBACK_OK
                                || status == Constants.MOTOR_STATUS_CALIB_OK)) {
                    lightUp(true);
                    playSoundEffect(Constants.OPEN_CAMERA_STATE);
                    mMotor.popupMotor(1);
                    mSensorManager.registerListener(
                            mFreeFallListener, mFreeFallSensor, SensorManager.SENSOR_DELAY_NORMAL);
                } else if (cameraState.equals(Constants.CLOSE_CAMERA_STATE)
                        && status == Constants.MOTOR_STATUS_POPUP_OK) {
                    lightUp(false);
                    playSoundEffect(Constants.CLOSE_CAMERA_STATE);
                    mMotor.takebackMotor(1);
                    mSensorManager.unregisterListener(mFreeFallListener, mFreeFallSensor);
                } else {
                    mMotorBusy = false;
                    if (status == Constants.MOTOR_STATUS_POPUP_JAMMED
                            || status == Constants.MOTOR_STATUS_TAKEBACK_JAMMED
                            || status == Constants.MOTOR_STATUS_CALIB_ERROR
                            || status == Constants.MOTOR_STATUS_REQUEST_CALIB) {
                        showErrorDialog();
                    }
                    return;
                }
            } catch (RemoteException e) {
                // Do nothing
            }
            mHandler.postDelayed(() -> mMotorBusy = false, 1200);
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

    private void setLed(String mode, String value) {
        FileUtils.writeLine(Constants.LEFT_LED_PATH + mode, value);
        FileUtils.writeLine(Constants.RIGHT_LED_PATH + mode, value);
    }

    private void lightUp(boolean open) {
        if (mPopupCameraPreferences.isLedAllowed()) {
            // Save the active LED effects if it's not our effect and mark the LED as busy
            if (!mLedBusy) {
                mLedBreathing = FileUtils.readOneLine(Constants.LEFT_LED_BREATH_PATH).equals("1");
                mLedBrightness = FileUtils.readOneLine(Constants.LEFT_LED_BRIGHTNESS_PATH);
            }
            mLedBusy = true;

            // Reset the active LED effects
            setLed("breath", "0");
            setLed("brightness", "0");

            // Tune the breath parameters for popup
            setLed("lo_idx", open ? "0" : "22");
            setLed("pause_lo_count", open ? "5" : "0");
            setLed("step_ms", "35");
            setLed("lut_pattern", "1");

            // Enable the breath effect
            setLed("breath", "1");

            mHandler.postDelayed(() -> {
                // Disable the breath effect
                setLed("brightness", "0");

                // Restore default breath parameters
                setLed("lo_idx", "0");
                setLed("pause_lo_count", "30");
                setLed("step_ms", "70");

                // Restore the previous LED effects
                if (mLedBreathing) {
                    FileUtils.writeLine(Constants.LEFT_LED_BREATH_PATH, "1");
                } else {
                    FileUtils.writeLine(Constants.LEFT_LED_BRIGHTNESS_PATH, mLedBrightness);
                }

                // Unmark the LED as busy since our effect is done
                mLedBusy = false;
            }, 1400);
        }
    }

    private void showCalibrationResult(int status) {
        if (mErrorDialogShowing) {
            return;
        }
        mErrorDialogShowing = true;
        mHandler.post(() -> {
            Resources res = getResources();
            int dialogMessageResId = mMotorCalibrating
                    ? R.string.popup_camera_calibrate_running
                    : (status == Constants.MOTOR_STATUS_CALIB_OK
                                    ? R.string.popup_camera_calibrate_success
                                    : R.string.popup_camera_calibrate_failed);
            AlertDialog.Builder alertDialogBuilder =
                    new AlertDialog.Builder(this, R.style.SystemAlertDialogTheme);
            alertDialogBuilder.setMessage(res.getString(dialogMessageResId));
            alertDialogBuilder.setPositiveButton(android.R.string.ok, null);
            AlertDialog alertDialog = alertDialogBuilder.create();
            alertDialog.getWindow().setType(WindowManager.LayoutParams.TYPE_KEYGUARD_DIALOG);
            alertDialog.setCancelable(false);
            alertDialog.setCanceledOnTouchOutside(false);
            alertDialog.show();
            alertDialog.setOnDismissListener(new DialogInterface.OnDismissListener() {
                @Override
                public void onDismiss(DialogInterface dialogInterface) {
                    mErrorDialogShowing = false;
                }
            });
        });
    }

    private void showErrorDialog() {
        if (mErrorDialogShowing) {
            return;
        }
        mErrorDialogShowing = true;
        goBackHome();
        mHandler.post(() -> {
            Resources res = getResources();
            String cameraState = "-1";
            int dialogMessageResId = cameraState.equals(Constants.CLOSE_CAMERA_STATE)
                    ? R.string.popup_camera_takeback_failed_calibrate
                    : R.string.popup_camera_popup_failed_calibrate;
            AlertDialog alertDialog =
                    new AlertDialog.Builder(this, R.style.SystemAlertDialogTheme)
                            .setTitle(res.getString(R.string.popup_camera_tip))
                            .setMessage(res.getString(dialogMessageResId))
                            .setPositiveButton(res.getString(R.string.popup_camera_calibrate_now),
                                    (dialog, which) -> calibrateMotor())
                            .setNegativeButton(res.getString(android.R.string.cancel), null)
                            .create();
            alertDialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
            alertDialog.setCanceledOnTouchOutside(false);
            alertDialog.show();
            alertDialog.setOnDismissListener(new DialogInterface.OnDismissListener() {
                @Override
                public void onDismiss(DialogInterface dialogInterface) {
                    mErrorDialogShowing = false;
                }
            });
        });
    }

    private void playSoundEffect(String state) {
        AudioManager audioManager =
                (AudioManager) getApplicationContext().getSystemService(Context.AUDIO_SERVICE);
        if (audioManager.getRingerMode() != AudioManager.RINGER_MODE_NORMAL) {
            return;
        }
        int soundEffect = Integer.parseInt(mPopupCameraPreferences.getSoundEffect());
        if (soundEffect != -1) {
            if (state.equals(Constants.CLOSE_CAMERA_STATE)) {
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

    @Override
    public boolean handleMessage(Message msg) {
        switch (msg.what) {
            case Constants.MSG_CAMERA_CLOSED: {
                if (mAlertDialog != null && mAlertDialog.isShowing()) {
                    mAlertDialog.dismiss();
                }
                updateMotor(Constants.CLOSE_CAMERA_STATE);
            } break;
            case Constants.MSG_CAMERA_OPEN: {
            boolean alwaysOnDialog = Settings.System.getInt(getContentResolver(),
                        alwaysOnDialogKey, 0) == 1;
            if (alwaysOnDialog || !mScreenOn) {
                updateDialogTheme();
                if (mAlertDialog == null) {
                    mAlertDialog = new AlertDialog.Builder(this, mDialogThemeResID)
                            .setMessage(R.string.popup_camera_dialog_message)
                            .setNegativeButton(R.string.popup_camera_dialog_no, (dialog, which) -> {
                            goBackHome();
                        })
                    .setPositiveButton(R.string.popup_camera_dialog_raise, (dialog, which) -> {
                    updateMotor(Constants.OPEN_CAMERA_STATE);
                        })
                        .create();
                    mAlertDialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ERROR);
                    mAlertDialog.setCanceledOnTouchOutside(false);
                }
                mAlertDialog.show();
            } else
                updateMotor(Constants.OPEN_CAMERA_STATE);
            } break;
        }
        return true;
    }

    private void updateDialogTheme() {
        int nightModeFlags = getResources().getConfiguration().uiMode
                & Configuration.UI_MODE_NIGHT_MASK;
        int themeResId;
        if (nightModeFlags == Configuration.UI_MODE_NIGHT_YES)
            themeResId = android.R.style.Theme_DeviceDefault_Dialog_Alert;
        else
            themeResId = android.R.style.Theme_DeviceDefault_Light_Dialog_Alert;
        if (mDialogThemeResID != themeResId) {
            mDialogThemeResID = themeResId;
            // if the theme changed force re-creating the dialog
            mAlertDialog = null;
        }
    }
}
