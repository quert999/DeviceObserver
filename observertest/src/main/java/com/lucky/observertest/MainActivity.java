package com.lucky.observertest;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.lucky.devicelib.DeviceObserver;
import com.lucky.devicelib.IServerCallback;

import java.util.List;

import pub.devrel.easypermissions.EasyPermissions;


public class MainActivity extends Activity{


    // Storage Permissions
    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
    };


    private void checkPermission() {
        // Check if we have write permission
        int permission = ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (permission != PackageManager.PERMISSION_GRANTED) {

            // We don't have permission so prompt the user
            ActivityCompat.requestPermissions(
                    this,
                    PERMISSIONS_STORAGE,
                    REQUEST_EXTERNAL_STORAGE
            );
        }
    }


    private Button btn_get_id;
    private TextView tv_device_info;
    private Button  btn_get_id_only;
    private TextView  tv_device_info_only;

    Object o;
    DeviceObserver deviceObserver = new DeviceObserver();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tv_device_info = findViewById(R.id.tv_device_info);
        btn_get_id = findViewById(R.id.btn_get_id);
        btn_get_id_only = findViewById(R.id.btn_get_id_only);
        tv_device_info_only = findViewById(R.id.tv_device_info_only);

        btn_get_id.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                checkPermission();
                tv_device_info.setText("id 生成中，请稍候");
                deviceObserver.queryDeviceMsg(new IServerCallback() {
                    @Override
                    public void onMessage(String id) {
                        tv_device_info.setText(id);
                    }
                });
            }
        });

        btn_get_id_only.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                deviceObserver.queryDeviceId(new IServerCallback() {
                    @Override
                    public void onMessage(String id) {
                        tv_device_info_only.setText(id);
                    }
                });
            }
        });

    }


}
