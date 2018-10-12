package com.lucky.observertest;

import android.app.Application;

import com.lucky.devicelib.DeviceObserver;
import com.tencent.bugly.crashreport.CrashReport;

import static android.os.Build.ID;


/**
 * Created by Administrator on 2018/9/19.
 */

public class TestApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        DeviceObserver.init(this);

        CrashReport.initCrashReport(getApplicationContext(), "742e10e761", false);
    }
}
