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

import android.app.AlertDialog;
import android.app.Activity;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.CheckBox;

import androidx.preference.Preference;
import androidx.preference.Preference.OnPreferenceChangeListener;
import androidx.preference.Preference.OnPreferenceClickListener;
import androidx.preference.PreferenceFragment;

import org.lineageos.settings.R;

public class PopupCameraSettingsFragment extends PreferenceFragment
        implements OnPreferenceChangeListener, OnPreferenceClickListener {
    private Preference mCalibrationPreference;
    private static final String MOTOR_CALIBRATION_KEY = "motor_calibration";

    private PopupCameraService mPopupCameraService = new PopupCameraService();

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.popup_settings);
        getActivity().getActionBar().setDisplayHomeAsUpEnabled(true);

        mCalibrationPreference = (Preference) findPreference(MOTOR_CALIBRATION_KEY);
        mCalibrationPreference.setOnPreferenceClickListener(this);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        return false;
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
            SharedPreferences prefs = getActivity().getSharedPreferences(
                    MOTOR_CALIBRATION_KEY, Activity.MODE_PRIVATE);
            if (!prefs.getBoolean("popup_calibration_warning_hidden", false)) {
                MotorCalibrationWarningDialog fragment = new MotorCalibrationWarningDialog();
                fragment.show(getFragmentManager(), "motor_calibration_warning_dialog");
            } else {
                mPopupCameraService.calibrateMotor();
            }
            return true;
        }
        return false;
    }

    private class MotorCalibrationWarningDialog extends DialogFragment {
        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            View view = getActivity().getLayoutInflater().inflate(
                    R.layout.popup_calibration_warning, null);
            CheckBox hideDialog = (CheckBox) view.findViewById(R.id.popup_calibration_warning_hide);

            hideDialog.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean isChecked) {
                    getActivity()
                            .getSharedPreferences(MOTOR_CALIBRATION_KEY, Activity.MODE_PRIVATE)
                            .edit()
                            .putBoolean("popup_calibration_warning_hidden", isChecked)
                            .commit();
                }
            });

            return new AlertDialog.Builder(getActivity())
                    .setTitle(R.string.popup_calibration_warning_title)
                    .setMessage(R.string.popup_calibration_warning_text)
                    .setView(view)
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
