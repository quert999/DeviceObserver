package com.lucky.devicelib;

public class ProcessThread extends Thread {

    String deviceInfo;

    public ProcessThread() {

        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }



    }


}
