package org.ifaa.android.manager;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Build;
import android.os.Build.VERSION;
import android.os.IBinder;
import android.os.IBinder.DeathRecipient;
import android.os.Parcel;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.util.Slog;
import java.util.ArrayList;
import java.util.Arrays;
import org.json.JSONObject;

public class IFAAManagerImpl extends IFAAManagerV3 {
    private static final boolean DEBUG = false;

    private static final int IFAA_TYPE_FINGER = 1;
    private static final int IFAA_TYPE_IRIS = (1 << 1);
    private static final int IFAA_TYPE_SENSOR_FOD = (1 << 4);

    private static final int ACTIVITY_START_SUCCESS = 0;
    private static final int ACTIVITY_START_FAILED = -1;

    private static volatile IFAAManagerImpl INSTANCE = null;

    private static final String TAG = "IfaaManagerImpl";

    private static ServiceConnection ifaaconn = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mService = IIFAAService.Stub.asInterface(service);
            try {
                mService.asBinder().linkToDeath(mDeathRecipient, 0);
            } catch (RemoteException e) {
                if (DEBUG) Slog.e(TAG, "linkToDeath fail.", e);
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            if (mContext != null) {
                if (DEBUG) Slog.i(TAG, "re-bind the service.");
                initService();
            }
        }
    };

    private static Context mContext = null;
    private static DeathRecipient mDeathRecipient = new DeathRecipient() {
        @Override
        public void binderDied() {
            if (mService != null) {
                if (DEBUG) Slog.d(TAG, "binderDied, unlink the service.");
                mService.asBinder().unlinkToDeath(mDeathRecipient, 0);
            }
        }
    };

    private static final String mIfaaActName = "org.ifaa.android.manager.IFAAService";
    private static final String mIfaaPackName = "org.ifaa.android.manager";
    private static IIFAAService mService = null;
    private static final String seperate = ",";
    private String mDevModel = null;

    public static IFAAManagerV3 getInstance(Context context) {
        if (INSTANCE == null) {
            synchronized (IFAAManagerImpl.class) {
                if (INSTANCE == null) {
                    INSTANCE = new IFAAManagerImpl();
                    if (VERSION.SDK_INT >= 28) {
                        mContext = context;
                        initService();
                    }
                }
            }
        }
        return INSTANCE;
    }

    private String initExtString() {
        String str = "";
        JSONObject location = new JSONObject();
        JSONObject fullView = new JSONObject();
        String xy = "";
        String wh = "";

        if (VERSION.SDK_INT >= 28) {
            xy = SystemProperties.get("persist.vendor.sys.fp.fod.location.X_Y", "");
            wh = SystemProperties.get("persist.vendor.sys.fp.fod.size.width_height", "");
        } else {
            xy = SystemProperties.get("persist.sys.fp.fod.location.X_Y", "");
            wh = SystemProperties.get("persist.sys.fp.fod.size.width_height", "");
        }

        try {
            if (validateVal(xy) && validateVal(wh)) {
                String[] splitXy = xy.split(seperate);
                String[] splitWh = wh.split(seperate);
                fullView.put("startX", Integer.parseInt(splitXy[0]));
                fullView.put("startY", Integer.parseInt(splitXy[1]));
                fullView.put("width", Integer.parseInt(splitWh[0]));
                fullView.put("height", Integer.parseInt(splitWh[1]));
                fullView.put("navConflict", true);
                location.put("type", 0);
                location.put("fullView", fullView);
                str = location.toString();
            } else {
                if (DEBUG) Slog.e(TAG, "initExtString invalidate, xy:" + xy + " wh:" + wh);
            }
        } catch (Exception e) {
            if (DEBUG) Slog.e(TAG, "Exception , xy:" + xy + " wh:" + wh, e);
        }
        return str;
    }

    private static void initService() {
        Intent intent = new Intent();
        intent.setClassName(mIfaaPackName, mIfaaActName);
        if (!mContext.bindService(intent, ifaaconn, Context.BIND_AUTO_CREATE)) {
            if (DEBUG) Slog.e(TAG, "cannot bind service org.ifaa.android.manager.IFAAService");
        }
    }

    private boolean validateVal(String str) {
        return !"".equalsIgnoreCase(str) && str.contains(",");
    }

    public String getDeviceModel() {
        if (mDevModel == null) {
            mDevModel = Build.MANUFACTURER + "-" + Build.DEVICE;
        }
        if (DEBUG) Slog.i(TAG, "getDeviceModel devcieModel:" + mDevModel);
        return mDevModel;
    }

    public String getExtInfo(int authType, String keyExtInfo) {
        return initExtString();
    }

    public int getSupportBIOTypes(Context context) {
        int ifaaType;
        String fpVendor = "";

        if (VERSION.SDK_INT >= 28) {
            ifaaType = SystemProperties.getInt("persist.vendor.sys.pay.ifaa", 0);
            fpVendor = SystemProperties.get("persist.vendor.sys.fp.vendor", "");
        } else {
            ifaaType = SystemProperties.getInt("persist.sys.ifaa", 0);
            fpVendor = SystemProperties.get("persist.sys.fp.vendor", "");
        }

        int supportBIOTypes = "none".equalsIgnoreCase(fpVendor) ? ifaaType & IFAA_TYPE_IRIS :
                ifaaType & (IFAA_TYPE_FINGER | IFAA_TYPE_IRIS);
        if ((supportBIOTypes & IFAA_TYPE_FINGER) == IFAA_TYPE_FINGER && sIsFod) {
            supportBIOTypes |= IFAA_TYPE_SENSOR_FOD;
        }
        if (DEBUG) Slog.i(TAG, "getSupportBIOTypes:" + ifaaType + " " + sIsFod + " " + fpVendor +
                " res:" + supportBIOTypes);
        return supportBIOTypes;
    }

    public int getVersion() {
        if (DEBUG) Slog.i(TAG, "getVersion sdk:" + VERSION.SDK_INT + " ifaaVer:" + sIfaaVer);
        return sIfaaVer;
    }

    public byte[] processCmdV2(Context context, byte[] data) {
        if (DEBUG) Slog.i(TAG, "processCmdV2 sdk:" + VERSION.SDK_INT);

        try {
            return mService.processCmd_v2(data);
        } catch (RemoteException e) {
            if (DEBUG) Slog.e(TAG, "processCmdV2 transact failed. " + e);
        }
        return null;
    }

    public void setExtInfo(int authType, String keyExtInfo, String valExtInfo) {
    }

    public int startBIOManager(Context context, int authType) {
        int res = ACTIVITY_START_FAILED;
        if (authType == IFAA_TYPE_FINGER) {
            Intent intent = new Intent("android.settings.SECURITY_SETTINGS");
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(intent);
            res = ACTIVITY_START_SUCCESS;
        }
        if (DEBUG) Slog.i(TAG, "startBIOManager authType:" + authType + " res:" + res);
        return res;
    }
}
