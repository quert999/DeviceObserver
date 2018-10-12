package com.lucky.devicelib;

import android.Manifest;
import android.app.Application;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import static android.content.Context.SENSOR_SERVICE;

/**
 * Created by Administrator on 2018/9/18.
 */

public class DeviceObserver{

    static {
        System.loadLibrary("DeviceObserver");
    }




    static DeviceObserver instance = new DeviceObserver();

    public DeviceObserver() {

    }

    // 标记尚未初始化
    private static boolean hasInit = false;
    public static Context appContext = null;


    public static DeviceObserver init(Application app) {

        if(hasInit){
            return null;
        }

        // 避免重复初始化
        appContext = app;
        // 初始化传感器信息
        instance.initSensor(app);

        hasInit = true;
        return null;
    }



    private SensorManager mSensorManager;
    private Sensor mAccelerometer;
    private Sensor mGyroScope;


    public static float xAccValue = 0;
    public static float yAccValue = 0;
    public static float zAccValue = 0;


    public static float xGyroValue = 0;
    public static float yGyroValue = 0;
    public static float zGyroValue = 0;


    private static int gyroTime = 0;
    private static int accTime = 0;


    SensorEventListener gyroEventListener;

    SensorEventListener sensorAccEventListener;

    /**
     * 初始化传感器信息
     */
    public void initSensor(Application app){

        sensorAccEventListener = new SensorEventListener() {
            @Override
            public void onSensorChanged(SensorEvent event) {

                if(event == null){
                    return ;
                }

                if(event.values == null){
                    return ;
                }

                if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                    if(event.values.length > 2){
                        xAccValue = event.values[0];
                        yAccValue = event.values[1];
                        zAccValue = event.values[2];

                        accTime += 1;
                        if(accTime >= 10) {
                            mSensorManager.unregisterListener(sensorAccEventListener);
                        }

                        if(xAccValue == 0 && yAccValue == 0 && zAccValue == 0){
                            return ;
                        }else{
                            mSensorManager.unregisterListener(sensorAccEventListener);
                        }


                    }
                }


            }

            @Override
            public void onAccuracyChanged(Sensor sensor, int accuracy) {

            }
        };

        gyroEventListener = new SensorEventListener() {

            @Override
            public void onSensorChanged(SensorEvent event) {

                if(event == null){
                    return ;
                }

                if(event.values == null){
                    return ;
                }
                if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {
                    if(event.values.length > 2){
                        xGyroValue = event.values[0];
                        yGyroValue = event.values[1];
                        zGyroValue = event.values[2];


                        gyroTime += 1;
                        if(gyroTime >= 10) {
                            mSensorManager.unregisterListener(gyroEventListener);
                        }

                        if(xGyroValue == 0 && yGyroValue == 0 && zGyroValue == 0){
                            return ;
                        }else{
                            mSensorManager.unregisterListener(gyroEventListener);
                        }


                    }
                }

            }

            @Override
            public void onAccuracyChanged(Sensor sensor, int accuracy) {

            }
        };

        mSensorManager = (SensorManager) app.getSystemService(SENSOR_SERVICE);

        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mGyroScope = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

        mSensorManager.registerListener(sensorAccEventListener, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
        mSensorManager.registerListener(gyroEventListener, mGyroScope, SensorManager.SENSOR_DELAY_NORMAL);

    }

    /**
     * 获取标准输出的设备信息
     *
     * @return
     */
    public native String getEncodeDeviceInfo();


    ServerHelper serverHelper = new ServerHelper(this);



    public String queryDeviceId(IServerCallback callback){

        String result = getEncodeDeviceInfo();
        // 发送请求
        serverHelper.uploadInfo(result,callback);
        return result;
    }

    public String queryDeviceMsg(IServerCallback callback){

        String result = getEncodeDeviceInfo();
        // 发送请求
        serverHelper.uploadInfoWithMsg(result,callback);
        return result;
    }

    /************************************  以下方法不可删除 *********************************************/
    /**
     * 判断字符串是否为null
     *
     * @param str
     * @return
     */
    public static boolean isStrNull(String str) {
        if (str == null || str.length() < 1) {
            return false;
        }
        return true;
    }

    /**
     * 获取应用程序名称
     */
    public synchronized String getAppName(Context context) {
        try {
            PackageManager packageManager = context.getPackageManager();
            PackageInfo packageInfo = packageManager.getPackageInfo(
                    context.getPackageName(), 0);
            int labelRes = packageInfo.applicationInfo.labelRes;
            return context.getResources().getString(labelRes);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }


    public static String getLinuxKernalInfoEx() {
        String result = "";
        String line;
        String[] cmd = new String[] { "/system/bin/cat", "/proc/version" };
        String workdirectory = "/system/bin/";
        try {
            ProcessBuilder bulider = new ProcessBuilder(cmd);
            bulider.directory(new File(workdirectory));
            bulider.redirectErrorStream(true);
            Process process = bulider.start();
            InputStream in = process.getInputStream();
            InputStreamReader isrout = new InputStreamReader(in);
            BufferedReader brout = new BufferedReader(isrout, 8 * 1024);

            while ((line = brout.readLine()) != null) {
                result += line;
                // result += "\n";
            }
            in.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    /**
     * 储存相关id信息
     * @param rootPath
     * @param assetManager
     * @param id
     * @return
     */
    private native boolean saveStoredId(String rootPath, AssetManager assetManager, String id);


    public boolean saveKey(String id){
        File rootFile = Environment.getExternalStorageDirectory();
        boolean saveResult = saveStoredId(rootFile.getPath(), appContext.getAssets(), id);
        return saveResult;
    }

}
