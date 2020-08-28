/*
 * Copyright (C) 2018 The LineageOS Project
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

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.provider.Settings;
import android.view.MenuItem;
import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.Preference.OnPreferenceClickListener;
import androidx.preference.PreferenceFragment;
import androidx.preference.SwitchPreference;

import org.lineageos.settings.R;

public class PopupCameraSettingsFragment extends PreferenceFragment
        implements OnPreferenceChangeListener, OnPreferenceClickListener {
    private Preference mCalibrationPreference;
    private SwitchPreference mAlwaysCameraSwitch;
    private static final String MOTOR_CALIBRATION_KEY = "motor_calibration";
    public static final String KEY_ALWAYS_CAMERA_DIALOG = "always_on_camera_dialog";

    private PopupCameraService mPopupCameraService = new PopupCameraService();

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.popup_settings);
        getActivity().getActionBar().setDisplayHomeAsUpEnabled(true);

        mCalibrationPreference = (Preference) findPreference(MOTOR_CALIBRATION_KEY);
        mCalibrationPreference.setOnPreferenceClickListener(this);

        mAlwaysCameraSwitch = (SwitchPreference) findPreference(KEY_ALWAYS_CAMERA_DIALOG);
        boolean enabled = Settings.System.getInt(getContext().getContentResolver(),KEY_ALWAYS_CAMERA_DIALOG, 0) == 1;
        mAlwaysCameraSwitch.setChecked(enabled);
        mAlwaysCameraSwitch.setOnPreferenceChangeListener(this);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        if (preference == mAlwaysCameraSwitch) {
            boolean enabled = (Boolean) newValue;
            Settings.System.putInt(getContext().getContentResolver(),
                KEY_ALWAYS_CAMERA_DIALOG,
                enabled ? 1 : 0);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            getActivity().onBackPressed();
            return true;
        }
        return false;
    }

    @Override
    public boolean onPreferenceClick(Preference preference) {
        if (MOTOR_CALIBRATION_KEY.equals(preference.getKey())) {
            MotorCalibrationWarningDialog fragment = new MotorCalibrationWarningDialog();
            fragment.show(getFragmentManager(), "motor_calibration_warning_dialog");
            return true;
        }
        return false;
    }

    private class MotorCalibrationWarningDialog extends DialogFragment {
        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            return new AlertDialog.Builder(getActivity())
                    .setTitle(R.string.popup_calibration_warning_title)
                    .setMessage(R.string.popup_calibration_warning_text)
                    .setPositiveButton(R.string.popup_camera_calibrate_now,
                            (dialog, which) -> {
                                mPopupCameraService.calibrateMotor();
                                dialog.cancel();
                            })
                    .setNegativeButton(android.R.string.cancel, (dialog, which) -> dialog.cancel())
                    .create();
        }
    }
}
