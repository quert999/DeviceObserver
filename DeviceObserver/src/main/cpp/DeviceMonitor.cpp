
#include <jni.h>
#include <string>
#include "./json/CJsonObject.hpp"
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/configuration.h>
#include "emulator.h"
#include "CBase64Coder.h"

#define LOG_TAG "dqqdo"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)


int checkJError(JNIEnv *env) {
    jthrowable err = env->ExceptionOccurred();
    if (err != NULL) {
        //手动清空异常信息，保证Java代码能够继续执行
        env->ExceptionClear();
        return -1;
    }
    return 1;
}


int isEmptyStr(JNIEnv *env, jstring str) {
    jclass clsstring = env->FindClass("com/lucky/devicelib/DeviceObserver");
    if (clsstring == NULL) {
        return -3;
    }
    jmethodID mid = env->GetStaticMethodID(clsstring, "isStrNull", "(Ljava/lang/String;)Z");
    if (mid == NULL) {
        return -2;
    }
    jboolean result = (jboolean) env->CallStaticBooleanMethod(clsstring, mid, str);
    if (result) {
        return 1;
    } else {
        return -1;
    }
}

std::string jstring2str(JNIEnv *env, jstring jstr) {

    if (isEmptyStr(env, jstr) < 0) {
        return "";
    }

    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");

    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);

    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    env->DeleteLocalRef(clsstring);
    env->DeleteLocalRef(strencode);
    return stemp;
}

jstring str2jstring(JNIEnv *env, const char *pat) {
    //定义java String类 strClass
    jclass strClass = (env)->FindClass("Ljava/lang/String;");
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    //建立byte数组
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
    //将char* 转换为byte数组
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte *) pat);
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
    jstring encoding = (env)->NewStringUTF("GB2312");
    //将byte数组转换为java String,并输出
    return (jstring) (env)->NewObject(strClass, ctorID, bytes, encoding);
}


/**
 * 获取系统的基础信息
 * @param env  jvm环境
 * @param key
 * @param def  defaultvalue
 * @return
 */
jstring getSysBaseInfo(JNIEnv *env, char *key, char *def) {

    try {
        jclass properClass = env->FindClass("android/os/SystemProperties");
        if (checkJError(env) < 0) {
            return env->NewStringUTF("JError");
        }
        jmethodID methodId = env->GetStaticMethodID(properClass, "native_get",
                                                    "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

        if (checkJError(env) < 0) {
            return env->NewStringUTF("JError2");
        }
        jstring jsKey = env->NewStringUTF(key);
        jstring jsDef = env->NewStringUTF(def);
        jobject result = env->CallStaticObjectMethod(properClass, methodId, jsKey, jsDef);
        if (checkJError(env) < 0) {
            return env->NewStringUTF("JError3");
        }
        jstring strResult = (jstring) result;

        env->DeleteLocalRef(properClass);
        env->DeleteLocalRef(jsDef);
        env->DeleteLocalRef(jsKey);

        return strResult;
    }
    catch (...) {
        if (checkJError(env) < 0) {
            return env->NewStringUTF("JError3");
        }
        return env->NewStringUTF("native error");
    }


}

/**
 * 获取包名
 * @param env
 * @param context
 * @return
 */
jstring getPackageName(JNIEnv *env, jobject context) {

    try {
        jclass contextClass = env->FindClass("android/content/Context");
        jmethodID getPackageNameId = env->GetMethodID(contextClass, "getPackageName",
                                                      "()Ljava/lang/String;");
        jstring packNameString = (jstring) env->CallObjectMethod(context, getPackageNameId);
        env->DeleteLocalRef(contextClass);

        return packNameString;
    } catch (...) {
        return env->NewStringUTF("nativeError");
    }


}

/**
 * 获取android id
 * @param env
 * @param mContext
 * @return
 */
jstring get_android_id(JNIEnv *env, jobject mContext) {

    if (mContext == NULL) {
        return env->NewStringUTF("");
    }

    try {
        jclass contextClass = env->FindClass("android/content/Context");
        jmethodID getMethod = env->GetMethodID(contextClass, "getContentResolver",
                                               "()Landroid/content/ContentResolver;");
        jobject resolver = env->CallObjectMethod(mContext, getMethod);

        if (resolver == NULL) {
            return env->NewStringUTF("");
        }

        jclass secClass = env->FindClass("android/provider/Settings$Secure");
        if (secClass == NULL) {
            return NULL;
        }

        jmethodID getStringMethod = env->GetStaticMethodID(secClass, "getString",
                                                           "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");
        if (getStringMethod == NULL) {
            return NULL;
        }

        jfieldID ANDROID_ID = env->GetStaticFieldID(secClass,
                                                    "ANDROID_ID", "Ljava/lang/String;");

        jstring str = (jstring) (env->GetStaticObjectField(secClass, ANDROID_ID));

        jstring jId = (jstring) (env->CallStaticObjectMethod(secClass, getStringMethod, resolver,
                                                             str));

        return jId;
    } catch (...) {
        return env->NewStringUTF("");
    }

}


/**
 * 获取包名的版本
 * @param env
 * @param context
 * @return
 */
neb::CJsonObject getPackageVersionInfo(JNIEnv *env, jobject context, neb::CJsonObject rootInfo) {

    try {
        // 获取pm对象
        jclass contextClass = env->FindClass("android/content/Context");
        jmethodID getPMId = env->GetMethodID(contextClass, "getPackageManager",
                                             "()Landroid/content/pm/PackageManager;");
        jobject pm = env->CallObjectMethod(context, getPMId);

        // 获取pi对象
        jclass pmClass = env->FindClass("android/content/pm/PackageManager");
        jmethodID getPIId = env->GetMethodID(pmClass, "getPackageInfo",
                                             "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
        jstring packageJS = getPackageName(env, context);

        jobject pi = env->CallObjectMethod(pm, getPIId, packageJS, 0);
        //获取版本信息
        jclass piClass = env->FindClass("android/content/pm/PackageInfo");

        jfieldID jfdVerCode = env->GetFieldID(piClass, "versionCode", "I");
        int versionCodeI = env->GetIntField(pi, jfdVerCode);
        char strVersionCode[64];
        sprintf(strVersionCode, "%d", versionCodeI);
        jstring versionCodeJS = env->NewStringUTF(strVersionCode);

        std::string strVersion = jstring2str(env, versionCodeJS);
        rootInfo.Add("appVersion", strVersion);

        jfieldID JidVN = env->GetFieldID(piClass, "versionName", "Ljava/lang/String;");
        jstring versionName = (jstring) env->GetObjectField(pi, JidVN);
        std::string verName = jstring2str(env, versionName);
        rootInfo.Add("buildVersion", verName);

        env->DeleteLocalRef(contextClass);
        env->DeleteLocalRef(pm);
        env->DeleteLocalRef(pmClass);
        env->DeleteLocalRef(packageJS);
        env->DeleteLocalRef(pi);
        env->DeleteLocalRef(piClass);

        return rootInfo;
    } catch (...) {
        return rootInfo;
    }

}


/**
 * 获取包名的版本
 * @param env
 * @param context
 * @return
 */
jstring getPackageVersionName(JNIEnv *env, jobject context) {

    try {
        // 获取pm对象
        jclass contextClass = env->FindClass("android/content/Context");
        jmethodID getPMId = env->GetMethodID(contextClass, "getPackageManager",
                                             "()Landroid/content/pm/PackageManager;");
        jobject pm = env->CallObjectMethod(context, getPMId);

        // 获取pi对象
        jclass pmClass = env->FindClass("android/content/pm/PackageManager");
        jmethodID getPIId = env->GetMethodID(pmClass, "getPackageInfo",
                                             "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
        jstring packageJS = getPackageName(env, context);

        jobject pi = env->CallObjectMethod(pm, getPIId, packageJS, 0);
        //获取版本信息
        jclass piClass = env->FindClass("android/content/pm/PackageInfo");

        jfieldID JidVN = env->GetFieldID(piClass, "versionName", "Ljava/lang/String;");
        jstring versionName = (jstring) env->GetObjectField(pi, JidVN);

        env->DeleteLocalRef(contextClass);
        env->DeleteLocalRef(pm);
        env->DeleteLocalRef(pmClass);
        env->DeleteLocalRef(packageJS);
        env->DeleteLocalRef(pi);
        env->DeleteLocalRef(piClass);

        return versionName;
    } catch (...) {
        return env->NewStringUTF("NativeError");
    }

}


/**
 * 获取app签名信息
 * @param env
 * @param mContext
 * @return
 */
jstring
get_sign_id(JNIEnv *env, jobject mContext) {

    try {

        jclass context_class = env->GetObjectClass(mContext);
        jmethodID methodId = env->GetMethodID(context_class, "getPackageManager",
                                              "()Landroid/content/pm/PackageManager;");
        jobject package_manager_object = env->CallObjectMethod(mContext, methodId);

        if (package_manager_object == NULL) {
            LOGD("getPackageManager() Failed!");
            return NULL;
        }

        //context.getPackageName()
        methodId = env->GetMethodID(context_class, "getPackageName", "()Ljava/lang/String;");
        jstring package_name_string = (jstring) env->CallObjectMethod(mContext, methodId);
        if (package_name_string == NULL) {
            LOGD("getPackageName() Failed!");
            return NULL;
        }

        env->DeleteLocalRef(context_class);


        jclass pack_manager_class = env->GetObjectClass(package_manager_object);
        methodId = env->GetMethodID(pack_manager_class, "getPackageInfo",
                                    "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
        env->DeleteLocalRef(pack_manager_class);
        jobject package_info_object = env->CallObjectMethod(package_manager_object, methodId,
                                                            package_name_string, 64);
        if (package_info_object == NULL) {
            return NULL;
        }


        env->DeleteLocalRef(package_manager_object);


        //PackageInfo.signatures[0]
        jclass package_info_class = env->GetObjectClass(package_info_object);
        jfieldID fieldId = env->GetFieldID(package_info_class, "signatures",
                                           "[Landroid/content/pm/Signature;");
        env->DeleteLocalRef(package_info_class);
        jobjectArray signature_object_array = (jobjectArray) env->GetObjectField(
                package_info_object,
                fieldId);
        if (signature_object_array == NULL) {
            return NULL;
        }
        jobject signature_object = env->GetObjectArrayElement(signature_object_array, 0);

        env->DeleteLocalRef(package_info_object);

        jclass signature_class = env->GetObjectClass(signature_object);
        methodId = env->GetMethodID(signature_class, "toCharsString", "()Ljava/lang/String;");
        env->DeleteLocalRef(signature_class);
        jstring signature_string = (jstring) env->CallObjectMethod(signature_object, methodId);
        return signature_string;
    } catch (...) {
        return env->NewStringUTF("signError");
    }


}

/**
 * 获取设备的imei
 * @param env
 * @param mContext
 * @return
 */
jstring getImei(
        JNIEnv *env,
        jobject mContext) {


    if (mContext == NULL) {
        return env->NewStringUTF("");
    }

    try {

        jclass cls_context = env->FindClass("android/content/Context");
        if (cls_context == NULL) {
            return env->NewStringUTF("[+] Error: FindClass <android/content/Context> Error");
        }

        jmethodID getSystemService = env->GetMethodID(cls_context, "getSystemService",
                                                      "(Ljava/lang/String;)Ljava/lang/Object;");

        if (getSystemService == NULL) {
            return env->NewStringUTF("");
        }

        jfieldID TELEPHONY_SERVICE = env->GetStaticFieldID(cls_context, "TELEPHONY_SERVICE",
                                                           "Ljava/lang/String;");

        if (TELEPHONY_SERVICE == NULL) {
            return env->NewStringUTF("");
        }

        jstring jsTelService = (jstring) env->GetStaticObjectField(cls_context, TELEPHONY_SERVICE);
        jobject telephonymanager = (env->CallObjectMethod(mContext, getSystemService,
                                                          jsTelService));

        if (checkJError(env) < 0) {
            return env->NewStringUTF("");
        }
        if (telephonymanager == NULL) {
            return env->NewStringUTF("");
        }

        jclass cls_TelephoneManager = env->FindClass("android/telephony/TelephonyManager");
        if (checkJError(env) < 0) {
            return env->NewStringUTF("");
        }
        if (cls_TelephoneManager == NULL) {
            return env->NewStringUTF("");
        }

        jmethodID getDeviceId = (env->GetMethodID(cls_TelephoneManager, "getDeviceId",
                                                  "()Ljava/lang/String;"));
        if (checkJError(env) < 0) {
            return env->NewStringUTF("");
        }

        jobject DeviceID = env->CallObjectMethod(telephonymanager, getDeviceId);
        if (checkJError(env) < 0) {
            return env->NewStringUTF("");
        }

        return (jstring) DeviceID;

    } catch (...) {
        return env->NewStringUTF("");
    }


}

/**
 * 执行系统命令 并返回结果
 * @param cmd 命令
 * @param result  执行结果
 */
void executeCMD(const char *cmd, char *result, int expectResultSize) {

    const int bufSize = 32;
    char buf_ps[bufSize];
    // 命令行数组（不能大于128）
    char ps[128] = {0};

    FILE *ptr;
    strcpy(ps, cmd);
    if ((ptr = popen(ps, "r")) != NULL) {
        while (fgets(buf_ps, bufSize, ptr) != NULL) {

            int existLen = strlen(result);
            int bufLen = strlen(buf_ps);
            if (existLen + bufLen > expectResultSize) {
                break;
            }
            //可以通过这行来获取shell命令行中的每一行的输出
            strcat(result, buf_ps);
        }
        pclose(ptr);
        ptr = NULL;
    } else {
        LOGD("popen %s error", ps);
    }
}

/**
 * 判断当前设备是否root
 * @return 是否root
 */
int isRoot() {

    try {
        const int expectResultSize = 256;
        char result[expectResultSize] = {0};

        // 查看su文件存在位置
        executeCMD("which su", result, expectResultSize);
        LOGD("result  %s", result);
        int resultSize = strlen(result);
        if (resultSize > 1) {
            return 1;
        }
        return -1;
    } catch (...) {
        return -2;
    }

}


/**
 * 获取内核办吧
 * @param env
 * @return
 */
jstring getCoreVersion(JNIEnv *env) {

    try {
        jclass clsstring = env->FindClass("com/lucky/devicelib/DeviceObserver");
        jmethodID mid = env->GetStaticMethodID(clsstring, "getLinuxKernalInfoEx",
                                               "()Ljava/lang/String;");
        jstring result = (jstring) env->CallStaticObjectMethod(clsstring, mid);
        return result;
    } catch (...) {
        return env->NewStringUTF("Error");
    }


}


/**
 * 获取ssid信息
 * @param env jni上下文环境
 * @param context android 上下文环境
 * @param infoObj 组装信息对象
 * @return
 */
neb::CJsonObject getSSIDInfo(JNIEnv *env,
                             jobject appContext,
                             neb::CJsonObject infoObj) {

    if (appContext == NULL) {
        return infoObj;
    }
    try {
        // 获取act,app,context
        jclass absContextClass = env->FindClass("android/content/Context");

        jfieldID wifiField = env->GetStaticFieldID(absContextClass, "WIFI_SERVICE",
                                                   "Ljava/lang/String;");
        jobject wikiNameObject = env->GetStaticObjectField(absContextClass, wifiField);
        jstring wifiServiceName = env->NewStringUTF("wifi");
        if (wikiNameObject != NULL) {
            wifiServiceName = (jstring) wikiNameObject;
        }

        jmethodID getWifiManagerMID = env->GetMethodID(absContextClass, "getSystemService",
                                                       "(Ljava/lang/String;)Ljava/lang/Object;");
        jobject wikiManager = env->CallObjectMethod(appContext, getWifiManagerMID, wifiServiceName);

        if (checkJError(env) < 0) {
            return infoObj;
        }
        if (wikiManager == NULL) {
            return infoObj;
        }

        // 获取wikiinfo
        jclass wikiManagerClass = env->FindClass("android/net/wifi/WifiManager");
        jmethodID getInfoMID = env->GetMethodID(wikiManagerClass, "getConnectionInfo",
                                                "()Landroid/net/wifi/WifiInfo;");
        jobject wikiInfo = env->CallObjectMethod(wikiManager, getInfoMID);

        if (checkJError(env) < 0) {
            return infoObj;
        }
        if (wikiInfo == NULL) {
            return infoObj;
        }

        // 获取ssid
        jclass wifiInfoClass = env->GetObjectClass(wikiInfo);
        jmethodID ssidMID = env->GetMethodID(wifiInfoClass, "getSSID", "()Ljava/lang/String;");
        jstring ssid = (jstring) env->CallObjectMethod(wikiInfo, ssidMID);

        // 获取ap mac
        jmethodID macMID = env->GetMethodID(wifiInfoClass, "getMacAddress", "()Ljava/lang/String;");
        jstring apMac = (jstring) env->CallObjectMethod(wikiInfo, macMID);

        // 获取ap mac
        jmethodID jmdBssidID = env->GetMethodID(wifiInfoClass, "getBSSID", "()Ljava/lang/String;");
        jstring jsBssid = (jstring) env->CallObjectMethod(wikiInfo, jmdBssidID);

        // 获取ip
        jmethodID getIpAddressM = env->GetMethodID(wifiInfoClass, "getIpAddress", "()I");
        int paramInt = (int) env->CallIntMethod(wikiInfo, getIpAddressM);
        char ip[15];
        sprintf(ip, "%d.%d.%d.%d", (paramInt & 0xFF), (0xFF & paramInt >> 8),
                (0xFF & paramInt >> 16),
                (0xFF & paramInt >> 24));

        // 获取设备的mac信息
        std::string strSsid = jstring2str(env, ssid);
        std::string strAPMac = jstring2str(env, apMac);
        std::string strBssid = jstring2str(env, jsBssid);

        infoObj.Add("deviceIp", ip);
        infoObj.Add("ssid", strSsid);
        infoObj.Add("bSsid", strBssid);
        infoObj.Add("mac", strAPMac);

        return infoObj;
    } catch (...) {
        return infoObj;
    }

}


/**
 * 获取传感器信息
 * @param env
 * @param instance
 * @param infoObj
 * @return
 */
neb::CJsonObject getSenorInfo(JNIEnv *env,
                              jobject instance,
                              neb::CJsonObject infoObj) {

    try {
        // 获取act,app,context
        jclass objectClass = env->GetObjectClass(instance);


        jfieldID xAccField = env->GetStaticFieldID(objectClass, "xAccValue",
                                                   "F");
        jfloat xAccValue = (jfloat) env->GetStaticFloatField(objectClass, xAccField);


        jfieldID yAccField = env->GetStaticFieldID(objectClass, "yAccValue",
                                                   "F");
        jfloat yAccValue = (jfloat) env->GetStaticFloatField(objectClass, yAccField);


        jfieldID zAccField = env->GetStaticFieldID(objectClass, "zAccValue",
                                                   "F");
        jfloat zAccValue = (jfloat) env->GetStaticFloatField(objectClass, zAccField);


        infoObj.AddEmptySubObject("accelerometerInfo");
        infoObj["accelerometerInfo"].Add("accelerationX", xAccValue);
        infoObj["accelerometerInfo"].Add("accelerationY", yAccValue);
        infoObj["accelerometerInfo"].Add("accelerationZ", zAccValue);


        jfieldID xGyroField = env->GetStaticFieldID(objectClass, "xGyroValue",
                                                    "F");
        jfloat xGyroValue = (jfloat) env->GetStaticFloatField(objectClass, xGyroField);

        jfieldID yGyroField = env->GetStaticFieldID(objectClass, "yGyroValue",
                                                    "F");
        jfloat yGyroValue = (jfloat) env->GetStaticFloatField(objectClass, yGyroField);

        jfieldID zGyroField = env->GetStaticFieldID(objectClass, "zGyroValue",
                                                    "F");

        jfloat zGyroValue = (jfloat) env->GetStaticFloatField(objectClass, zGyroField);


        infoObj.AddEmptySubObject("gyroInfo");
        infoObj["gyroInfo"].Add("rotationRateX", xGyroValue);
        infoObj["gyroInfo"].Add("rotationRateY", yGyroValue);
        infoObj["gyroInfo"].Add("rotationRateZ", zGyroValue);


        return infoObj;
    } catch (...) {
        return infoObj;
    }


}


neb::CJsonObject getHookInfo(JNIEnv *env,
                             jobject appContext,
                             neb::CJsonObject infoObj) {

    try {
        // 获取act,app,context
        jclass clsCheckHook = env->FindClass("com/lucky/devicelib/CheckHook");

        jmethodID midIsHook = env->GetStaticMethodID(clsCheckHook, "isHook",
                                                     "(Landroid/content/Context;)Z");

        jboolean isHook = env->CallStaticBooleanMethod(clsCheckHook, midIsHook, appContext);

        if (checkJError(env) < 0) {
            return infoObj;
        }
        infoObj.Add("isHook", isHook);

        return infoObj;
    } catch (...) {
        return infoObj;
    }


}

/**
 * 获取储存的本地的id
 * @param env
 * @param rootPath
 * @return
 */
string getStoredId(
        JNIEnv *env) {

    // 获取sd卡根目录

    jclass clsEnv = env->FindClass("android/os/Environment");
    jmethodID jmidRootFile = env->GetStaticMethodID(clsEnv,"getExternalStorageDirectory","()Ljava/io/File;");

    jobject joRootFile = env->CallStaticObjectMethod(clsEnv,jmidRootFile);
    jclass clsFile = env->GetObjectClass(joRootFile);

    jmethodID jmidGetPath = env->GetMethodID(clsFile,"getPath","()Ljava/lang/String;");
    jstring  jsRootPath = (jstring)(env->CallObjectMethod(joRootFile,jmidGetPath));


    if(checkJError(env) < 0){
        return "";
    }

    std::string rootPath = jstring2str(env,jsRootPath);


    /**
    * 约定的秘钥位置信息
    */
    const unsigned int writePos = 67447;
    const unsigned int keySize = 35;
    const unsigned int rand = 8;

    /**
    * 获取隐藏起来的id信息
    */
    std::string storedId = "";
    std::string fromPath = rootPath;
    std::string AssumeFilePath = fromPath + "/Runtime.so";

    if(access(AssumeFilePath.c_str(),F_OK) < 0){
        LOGD("access file error!");
        return "";
    }
    FILE *fp_to = NULL;
    //打开目标文件
    if ((fp_to = fopen(AssumeFilePath.c_str(), "r")) == NULL) {
        LOGD("open write file error!");
        return "";
    }


    fseek(fp_to, writePos, 0);
    char key[keySize];
    fread(key, keySize, 1, fp_to);
    LOGD("88888  %s----%d", key,strlen(key));
    for (int i = 0; i < keySize; i++) {
        key[i] += rand;
    }
    LOGD("99999999  %s---%d", key,strlen(key));
    return key;


}

/**
 * 获取设备原始信息
 * @param env
 * @param thisObj
 * @return
 */
std::string getDeviceInfoSrc(JNIEnv *env,
                             jobject thisObj) {

    /**
     * 平台标示 常量
     */
    const char *platform = "Android";
    // json 数据对象
    neb::CJsonObject infoObj;

    /******************************** 获取上下环境 ************************************/
    jclass observerClass = env->GetObjectClass(thisObj);
    jfieldID contextFieldId = env->GetStaticFieldID(observerClass, "appContext",
                                                    "Landroid/content/Context;");
    jobject appContextObj = env->GetStaticObjectField(observerClass, contextFieldId);


    /******************************** 获取基础config信息 ************************************/
    jclass conextClass = env->GetObjectClass(appContextObj);
    jmethodID getAssets = env->GetMethodID(conextClass, "getAssets",
                                           "()Landroid/content/res/AssetManager;");

    //创建一个AssetManager对象
    AAssetManager *mgr = AAssetManager_fromJava(env,
                                                env->CallObjectMethod(appContextObj, getAssets));
    // get config
    struct AConfiguration *initConfig = AConfiguration_new();
    AConfiguration_fromAssetManager(initConfig, mgr);
    int32_t sdkVersion = AConfiguration_getSdkVersion(initConfig);
    infoObj.Add("sdkVersion", sdkVersion);
    infoObj.Add("systemName", platform);
    int32_t screenHeight = AConfiguration_getScreenHeightDp(initConfig);
    infoObj.Add("screenHeight", screenHeight);
    int32_t screenWidth = AConfiguration_getScreenWidthDp(initConfig);
    infoObj.Add("screenWidth", screenWidth);

    char language[6];
    memset(language, 0, 6);
    AConfiguration_getLanguage(initConfig, language);
    infoObj.Add("systemLanguage", language);

    char countryCode[6];
    memset(countryCode, 0, 6);
    AConfiguration_getCountry(initConfig, countryCode);
    infoObj.Add("country", countryCode);


    int32_t mcc = AConfiguration_getMcc(initConfig);
    int32_t mnc = AConfiguration_getMnc(initConfig);
    infoObj.Add("mcc", mcc);
    infoObj.Add("mnc", mnc);
    AConfiguration_delete(initConfig);


    /*************************    获取基础配置信息   *****************************/
    jstring sysVersion = getSysBaseInfo(env, "ro.build.version.release", "");
    std::string strOSVersion = jstring2str(env, sysVersion);
    // 版本
    infoObj.Add("OSVerison", strOSVersion);
    // 型号
    jstring model = getSysBaseInfo(env, "ro.product.model", "");
    // 品牌
    jstring brand = getSysBaseInfo(env, "ro.product.brand", "");

    std::string strModel = jstring2str(env, model);
    std::string strBrand = jstring2str(env, brand);
    std::string strBAM = strBrand + "-" + strModel;
    infoObj.Add("deviceType", strBAM);

    // 内部版本号码
    jstring jsBoard = getSysBaseInfo(env, "ro.product.board", "");
    std::string strBoard = jstring2str(env, jsBoard);
    infoObj.Add("deviceModel", strBoard);

    // 生产商
    jstring jsProducter = getSysBaseInfo(env, "ro.product.manufacturer", "");
    std::string strProducter = jstring2str(env, jsProducter);
    infoObj.Add("manufacturers", strProducter);

    // 设备名称
    jstring jsDeviceName = getSysBaseInfo(env, "ro.product.device", "");
    std::string strDeviceName = jstring2str(env, jsDeviceName);
    infoObj.Add("deviceName", strDeviceName);

    /*******************************  获取其他特殊数据  *********************************/
    // 签名信息
    jstring jsSignId = get_sign_id(env, appContextObj);
    std::string strSignId = jstring2str(env, jsSignId);
    infoObj.Add("appSignature", strSignId);

    // 包名
    jstring jsPackageName = getPackageName(env, appContextObj);
    std::string strPackageName = jstring2str(env, jsPackageName);
    infoObj.Add("bundleId", strPackageName);


    /************************* 版本信息 ***************************/
    infoObj = getPackageVersionInfo(env, appContextObj, infoObj);



    // 应用名称
    jmethodID jmiAppName = env->GetMethodID(observerClass, "getAppName",
                                            "(Landroid/content/Context;)Ljava/lang/String;");
    jobject appName = env->CallObjectMethod(thisObj, jmiAppName, appContextObj);
    jstring jsAppName = (jstring) appName;
    std::string strAppName = jstring2str(env, jsAppName);
    infoObj.Add("appName", strAppName);


    // getImei
    jstring jsImei = getImei(env, appContextObj);
    std::string strImei = jstring2str(env, jsImei);
    infoObj.Add("imei", strImei);

    // 获取android ID
    jstring androidId = get_android_id(env, appContextObj);
    std::string strAID = jstring2str(env, androidId);
    infoObj.Add("androidId", strAID);

    // root
    int rootInfo = isRoot();
    infoObj.Add("root", rootInfo);

    // 基带版本
    jstring coreVersion = (jstring) getCoreVersion(env);
    std::string strCoreVersion = jstring2str(env, coreVersion);
    infoObj.Add("coreVersion", strCoreVersion);

    // 获取wifi info
    infoObj = getSSIDInfo(env, appContextObj, infoObj);

    // emulator check
    std::string emulatorResult = "";

    emulatorResult = detectIsEmulator(env, emulatorResult);
    if (checkJError(env) < 0) {
        infoObj.Add("isSimulator", "error");
    } else {
        infoObj.Add("isSimulator", emulatorResult);
    }

    // 拼装陀螺仪物理数据
    infoObj = getSenorInfo(env, thisObj, infoObj);
    // 监测hook信息
    infoObj = getHookInfo(env, appContextObj, infoObj);
    // storeid 信息
    std::string storeId = getStoredId(env);
    storeId = storeId.substr(0,35);

    infoObj.Add("storeId",storeId);
    std::string strResult = infoObj.ToString();

    if (checkJError(env) < 0) {
        strResult = "";
    }

    return strResult;
}


/**
 * 对字符串进行加密
 * @param c
 * @param a
 */
void encryption(std::string &c, int a[]) {

    for (int i = 0, j = 0; c[j]; j++, i = (i + 1) % 7) {
        c[j] ^= a[i];
    }

}

/**
 * 对字符串进行解密
 * @param c
 * @param a
 */
void decode(std::string &c, int a[]) {
    for (int i = 0, j = 0; c[j]; j++, i = (i + 1) % 7) {
        c[j] ^= a[i];
    }
}

#define JNIREG_CLASS "com/lucky/devicelib/DeviceObserver"

__attribute__((section (".mytext"))) JNICALL jboolean saveDeviceStoredId(
        JNIEnv *env,
        jobject,
        jstring rootPath,
        jobject assetManager,
        jstring devId) {

    std::string sId = jstring2str(env, devId);
    LOGD("30000000 %s", sId.c_str());
    /**
     * 常量偏移值
     */
    const int rand = 8;
    /**
     * 首先是拼接目标位置路径
     */
    std::string fromPath = env->GetStringUTFChars(rootPath, 0);
    std::string AssumeFilePath = fromPath + "/Runtime.so";
    std::string srcFileName = "demo_logo.png";
    std::string idStr = env->GetStringUTFChars(devId, 0);

    /**
     * 检查拼接完成的文件位置是否可写入
     */
    FILE *fp_to;
    if ((fp_to = fopen(AssumeFilePath.c_str(), "w")) == NULL) {
        return false;
    }
    // 该地址可以写入，则从asset目录中读取原始文件，混合id后写入新文件
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    if (mgr == NULL) {
        // asset目录获取异常
        return false;
    }

    AAsset *asset = AAssetManager_open(mgr, srcFileName.c_str(), AASSET_MODE_UNKNOWN);

    if (asset == NULL) {
        return false;
    }

    /**
     * 单次写入buffer
     */
    const int perBuffer = 256;
    /**
     * 已写入大小
     */
    int writeSize = 0;
    /**
     * 字符缓存数组
     */
    char *buffer = (char *) malloc(perBuffer);

    int numBytesRead = -1;
    do {
        memset(buffer, 0, perBuffer);
        numBytesRead = AAsset_read(asset, buffer, perBuffer);

        fwrite(buffer, 1, numBytesRead, fp_to);

        writeSize += numBytesRead;
    } while (numBytesRead > 0);


    // 获取id字符串
    unsigned int iSize = idStr.size();
    unsigned int i = 0;

    for (i = 0; i < iSize; i++) {
        idStr[i] -= rand;
    }
    std::string blockStr = "gIFt" + idStr;

    fwrite(idStr.c_str(), 1, idStr.length(), fp_to);
    fwrite("\0", 1, 1, fp_to);
    // 扫尾
    fflush(fp_to);
    free(buffer);
    /*关闭文件*/
    fclose(fp_to);
    AAsset_close(asset);


    // 返回加密结果标记
    std::string success = std::to_string(writeSize) + "-"
                          + std::to_string(idStr.length()) + "-" +
                          std::to_string(rand);
    LOGD("success: %s", success.c_str());
    return true;


}

__attribute__((section (".mytext"))) JNICALL jstring getEncodeInfo(JNIEnv *env,
                                                                   jobject thisObj) {

    std::string strResult = getDeviceInfoSrc(env, thisObj);
    CBase64Coder cBase64Coder;
    std::string encodedResult = cBase64Coder.encode(strResult);


    int a[] = {4, 9, 6, 2, 8, 7, 3};

    encryption(encodedResult, a);

    std::string sFlag = "a8;";
    std::string dFlag = "d0az";
    strResult = sFlag + encodedResult + dFlag;


    return env->NewStringUTF(strResult.c_str());
}

static JNINativeMethod getMethods[] = {
        {"getEncodeDeviceInfo", "()Ljava/lang/String;",                                                      (void *) getEncodeInfo},
        {"saveStoredId",        "(Ljava/lang/String;Landroid/content/res/AssetManager;Ljava/lang/String;)Z", (void *) saveDeviceStoredId},
};

//此函数通过调用JNI中 RegisterNatives 方法来注册我们的函数
static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *getMethods,
                                 int methodsNum) {
    jclass clazz;
    //找到声明native方法的类
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    //注册函数 参数：java类 所要注册的函数数组 注册函数的个数
    if (env->RegisterNatives(clazz, getMethods, methodsNum) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerNatives(JNIEnv *env) {
    return registerNativeMethods(env, JNIREG_CLASS, getMethods,
                                 sizeof(getMethods) / sizeof(getMethods[0]));
}
//回调函数 在这里面注册函数
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    //判断虚拟机状态是否有问题
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    assert(env != NULL);
    //开始注册函数 registerNatives -》registerNativeMethods -》env->RegisterNatives
    if (!registerNatives(env)) {
        return -1;
    }
    //返回jni 的版本
    return JNI_VERSION_1_6;
}


