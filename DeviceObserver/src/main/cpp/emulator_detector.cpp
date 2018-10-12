#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>
#include <string>
#include <android/log.h>
#include <sys/stat.h>
#include <string.h>
#include "stdlib.h"
#include "unistd.h"
#include "stdio.h"
#include<dirent.h>

#include "emulator.h"


//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define LOG_TAG "dqqdo"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)



std::string detectIsEmulator(JNIEnv *env,std::string result) {

    result = "";

    //特殊机型检测
    if (is_special_emulator()) {
        result = result + "-SE";
    }

    //基带
    if(!check_contain_BaseBand(env)){
        result = result + "-NBB";
    }

    //蓝牙
    if(!check_contain_bluetooth(env)){
        result = result + "-NBT";
    }

    //CPU
    if(!check_contain_cpu_Temperature()){
        result = result + "-NCT";
    }

    return result;

}

/**
 * 基带检测
 * 基带是手机上的一块电路板，因为模拟器没有真实的电路板（基带电路）
 * 部分真机在刷机失败的时候也会出现丢失基带的情况
 */
bool check_contain_BaseBand(JNIEnv *env) {

    jclass systemClass = env->FindClass("android/os/SystemProperties");

    jmethodID getMethodID = env->GetStaticMethodID(systemClass, "get",
                                                   "(Ljava/lang/String;)Ljava/lang/String;");

    jstring  key = env->NewStringUTF("gsm.version.baseband");
    jstring baseBand = (jstring)(env->CallStaticObjectMethod(systemClass,getMethodID,key));

    if(baseBand == NULL || env->GetStringLength(baseBand) == 0){
        return false;
    } else{
        const char *dname = env->GetStringUTFChars(baseBand, 0);
        LOGD("BASEBAND NAME  =  %s", dname);
        return true;
    }
}

/**
 * CPU温度文件检测
 *真机 thermal_zoneX
 *模拟器 cooling_deviceX  模拟器上没有thermal_zoneX目录
 */
bool check_contain_cpu_Temperature() {
    const char* path = "/sys/class/thermal/";

    DIR *dirptr = NULL;
    struct dirent *entry;
    if ((dirptr = opendir(path)) != NULL) {
        while ((entry = readdir(dirptr))) {
            // LOGE("%s  \n", entry->d_name);
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }
            char *tmp = entry->d_name;
            if (strstr(tmp, "thermal_zone") != NULL) {
                return true;
            }
        }
        closedir(dirptr);
        return false;
    } else {
        //没有文件列表-》权限问题
        //模拟器和真机存在权限问题,则返回true的情况
        LOGD("Open thermal Fail");
        return true;
    }
}

/**
 * 是否有蓝牙
 */
bool check_contain_bluetooth(JNIEnv *env) {
    //模拟器是没有蓝牙功能的，libbluetooth_jni.so在模拟器上一般是不存在的
    // 荣耀10没有此文件，可能权限问题或其他，逍遥游上都有此文件，暂时补不能作为判断的依据
    // String bluePath = "/system/lib/libbluetooth_jni.so";
    // new File(bluePath).exists());

    jclass blueClass = env->FindClass("android/bluetooth/BluetoothAdapter");

    //blueClass
    jmethodID blueMethodID = env->GetStaticMethodID(blueClass, "getDefaultAdapter",
                                                    "()Landroid/bluetooth/BluetoothAdapter;");

    //通过判断蓝牙
    jobject result = env->CallStaticObjectMethod(blueClass, blueMethodID);
    if (result == NULL) {
        LOGD("BluetoothAdapter NULL");
        return false;
    } else {
        //注意，需要蓝牙权限android.permission.BLUETOOTH
        jmethodID jmethodName = env->GetMethodID(blueClass, "getName", "()Ljava/lang/String;");

        jstring resultStr = (jstring) (env->CallObjectMethod(result, jmethodName));

        if (resultStr == NULL || env->GetStringLength(resultStr) == 0) {
            return false;
        } else {
            const char *dname = env->GetStringUTFChars(resultStr, 0);
            LOGD("BLUETOOTH NAME  =  %s", dname);
            return true;
        }
    }
}


/**
 * 是否包含特殊标记的模拟器
 * @return
 */
bool is_special_emulator() {
    const char *filePath[] = {
            "/system/bin/nemuVM-prop", //网易mumu
            "/system/bin/androVM-prop", //Genymotion、腾讯手游助手
            "/system/bin/ttVM-prop", //天天
            "/system/bin/3btrans", //51模拟器、蓝叠
            "/system/bin/droid4x-prop", //海马
            "/ueventd.nox.rc", //夜神(Windows)
            "/init.nox.rc", //夜神(MAC)
    };
    int len = sizeof(filePath) / sizeof(filePath[0]);

    for (int i = 0; i < len; ++i) {
        if (detector_file_exists(filePath[i])) {
            return true;
        }
    }
    return false;
}


//文件是否存在
bool detector_file_exists(const char *filename) {
    struct stat statbuf;
    //执行成功则返回0，失败返回-1
    int result = stat(filename, &statbuf) == 0 ? 1 : 0;
    if (result) {
        LOGD("detector_file_exists  %s exists", filename);
        return true;
    }
    return false;
}

//----------------------------模拟器检测  end------------------------


