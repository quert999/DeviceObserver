package com.lucky.devicelib;


import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;

import com.lucky.devicelib.net.HttpConnectionUtil;


import org.json.JSONException;

import java.util.HashMap;
import java.util.Map;


public class ServerHelper {

    /**
     * server 接口地址
     */
    final String URLSERVER = "https://www.dqqdo.com//StockHelper/d/p";
    final int NORMAL_ID_LENGTH = 35;
    String erwinId = "";
    DeviceObserver deviceObserver;


    ServerHelper(DeviceObserver deviceO){
        this.deviceObserver = deviceO;
    }




    boolean uploadInfo(String uploadInfo, final IServerCallback callback){

        final Handler handler = new Handler(Looper.getMainLooper());


        Map param = new HashMap(2);
        param.put("info", uploadInfo);
        String response = HttpConnectionUtil.getHttp().postRequset(URLSERVER,param);

        if(TextUtils.isEmpty(response)){
            erwinId = "";
        }

        org.json.JSONObject jsonObject;
        try {
            jsonObject = new org.json.JSONObject(response);
            erwinId = jsonObject.getString("id");
        } catch (JSONException e) {
            erwinId = "";
        }



        handler.post(new Runnable() {
            @Override
            public void run() {
                callback.onMessage(erwinId);

                if(!TextUtils.isEmpty(erwinId) && erwinId.length() >= NORMAL_ID_LENGTH){
                    String saveId = erwinId.substring(0,NORMAL_ID_LENGTH);
                    deviceObserver.saveKey(saveId);
                }

            }
        });

        return true;

    }


    /**
     * 返回id + msg
     * @param uploadInfo
     * @param callback
     * @return
     */
    boolean uploadInfoWithMsg(String uploadInfo, final IServerCallback callback){

        final Handler handler = new Handler(Looper.getMainLooper());


        Map param = new HashMap(2);
        param.put("info", uploadInfo);
        String response = HttpConnectionUtil.getHttp().postRequset(URLSERVER,param);

        if(TextUtils.isEmpty(response)){
            erwinId = "";
        }

        org.json.JSONObject jsonObject;
        try {
            jsonObject = new org.json.JSONObject(response);
            erwinId = jsonObject.getString("id");
            String msg = jsonObject.getString("msg");
            erwinId += "---msg:" + msg;
        } catch (JSONException e) {
            erwinId = "";
        }


        handler.post(new Runnable() {
            @Override
            public void run() {
                callback.onMessage(erwinId);

                if(!TextUtils.isEmpty(erwinId) && erwinId.length() >= NORMAL_ID_LENGTH){

                    String saveId = erwinId.substring(0,NORMAL_ID_LENGTH);
                    deviceObserver.saveKey(saveId);
                }

            }
        });

        return true;

    }



}
