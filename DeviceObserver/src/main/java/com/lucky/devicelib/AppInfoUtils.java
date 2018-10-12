package com.lucky.devicelib;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Build;
import android.telephony.TelephonyManager;
import android.text.format.Formatter;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.List;

/**
 * @author : 杜宗宁
 * @date : 2018/9/6
 * 获取所有app列表，app签名相关信息工具类
 */
public class AppInfoUtils {

    public final static String SHA1 = "SHA1";
    public final static String MD5 = "MD5";
    public final static String SHA256 = "SHA256";

    /**
     * 获取设备信息
     */
    private String getDeviceInfo() {
        StringBuilder sb = new StringBuilder();
        sb.append("主板：").append(Build.BOARD);
        sb.append("，系统启动程序版本号：").append(Build.BOOTLOADER);
        sb.append("，系统定制商：").append(Build.BRAND);
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            sb.append(",cpu指令集 ").append(Build.CPU_ABI);
        } else {
            String[] abis = Build.SUPPORTED_ABIS;
            sb.append("，cpu总指令集 ");
            for (String string : abis) {
                sb.append(string).append(",");
            }
        }
        sb.append("，设置参数：").append(Build.DEVICE);
        sb.append("，显示屏参数：").append(Build.DISPLAY);
        sb.append("，无线电固件版本：").append(Build.getRadioVersion());
        sb.append("，硬件识别码： ").append(Build.FINGERPRINT);
        sb.append("，硬件名称： ").append(Build.HARDWARE);
        sb.append("，HOST: ").append(Build.HOST);
        sb.append("，修订版本列表：").append(Build.ID);
        sb.append("，硬件制造商：").append(Build.MANUFACTURER);
        sb.append("，版本：").append(Build.MODEL);
        sb.append("，硬件序列号：").append(Build.SERIAL);
        sb.append("，手机制造商： ").append(Build.PRODUCT);
        sb.append("，描述Build的标签：").append(Build.TAGS);
        sb.append("，TIME: ").append(Build.TIME);
        sb.append("，builder类型：").append(Build.TYPE);
        sb.append("，USER: ").append(Build.USER);
        return sb.toString();
    }

    /**
     * 获取三方应用的包信息
     */
    public static List<PackageInfo> getThirdAppList(Context context) {
        PackageManager packageManager = context.getPackageManager();
        List<PackageInfo> packageInfoList = packageManager.getInstalledPackages(0);
        List<PackageInfo> thirdAPP = new ArrayList<>();
        for (int i = 0; i < packageInfoList.size(); i++) {
            PackageInfo pak = packageInfoList.get(i);
            //判断是否为系统预装的应用
            if ((pak.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) <= 0) {
                // 第三方应用
                thirdAPP.add(pak);
            }
        }
        return thirdAPP;
    }

    /**
     * 获取所有应用的包信息
     */
    public static List<PackageInfo> getAllAppList(Context context) {
        PackageManager packageManager = context.getPackageManager();
        return packageManager.getInstalledPackages(0);
    }

    /**
     * 获取系统应用的包信息
     */
    public static List<PackageInfo> getSystemAppList(Context context) {
        PackageManager packageManager = context.getPackageManager();
        List<PackageInfo> packageInfoList = packageManager.getInstalledPackages(0);
        // 判断是否系统应用：
        //List<PackageInfo> apps = new ArrayList<PackageInfo>();
        List<PackageInfo> systemApp = new ArrayList<>();
        for (int i = 0; i < packageInfoList.size(); i++) {
            PackageInfo pak = packageInfoList.get(i);
            //判断是否为系统预装的应用
            if ((pak.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) > 0) {
                systemApp.add(pak);
            }
        }
        return systemApp;
    }


    /**
     * 返回一个签名的对应类型的字符串
     */
    public static String getSingInfo(Context context, String packageName, String type) {
        String tmp = null;
        Signature[] signs = getSignatures(context, packageName);
        if (signs == null) {
            return "";
        }
        for (Signature sig : signs) {
            if (SHA1.equals(type) || MD5.equals(type) || SHA256.equals(type)) {
                tmp = getSignatureString(sig, type);
                break;
            }
        }
        return tmp;
    }

    /**
     * 返回对应包的签名信息
     */
    private static Signature[] getSignatures(Context context, String packageName) {
        PackageInfo packageInfo;
        try {
            packageInfo = context.getPackageManager().getPackageInfo(packageName, PackageManager.GET_SIGNATURES);
            return packageInfo.signatures;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return null;
    }

    /**
     * 获取相应的类型的字符串（把签名的byte[]信息转换成16进制）
     */
    private static String getSignatureString(Signature sig, String type) {
        byte[] hexBytes = sig.toByteArray();
        String fingerprint = "error!";
        try {
            MessageDigest digest = MessageDigest.getInstance(type);
            if (digest != null) {
                byte[] digestBytes = digest.digest(hexBytes);
                StringBuilder sb = new StringBuilder();
                for (byte digestByte : digestBytes) {
                    sb.append((Integer.toHexString((digestByte & 0xFF) | 0x100)).substring(1, 3));
                }
                fingerprint = sb.toString();
            }
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return fingerprint;
    }

    /**
     * 获取系统内存大小
     */
    public static String getSysteTotalMemorySize(Context context) {
        //获得ActivityManager服务的对象
        ActivityManager mActivityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        //获得MemoryInfo对象
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        //获得系统可用内存，保存在MemoryInfo对象上
        if (mActivityManager != null) {
            mActivityManager.getMemoryInfo(memoryInfo);
        }
        long memSize = memoryInfo.totalMem;
        //字符类型转换
        return formatFileSize(context, memSize);
    }

    /**
     * 获取系统可用的内存大小
     */
    public static String getSystemAvaialbeMemorySize(Context context) {
        //获得ActivityManager服务的对象
        ActivityManager mActivityManager = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        //获得MemoryInfo对象
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        //获得系统可用内存，保存在MemoryInfo对象上
        if (mActivityManager != null) {
            mActivityManager.getMemoryInfo(memoryInfo);
        }
        long memSize = memoryInfo.availMem;
        //字符类型转换
        return formatFileSize(context, memSize);
    }

    /**
     * 调用系统函数，字符串转换 long -String KB/MB
     */
    private static String formatFileSize(Context context, long size) {
        return Formatter.formatFileSize(context, size);
    }

    /**
     * 获取当前的运营商
     */
    public static String getOperator(Context context) {
        TelephonyManager tm = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
        String operator = "";
        if (tm != null) {
            operator = tm.getNetworkOperator();
        }
        String opeType = "";
        if ("46001".equals(operator) || "46006".equals(operator) || "46009".equals(operator)) {
            opeType = "中国联通";
        } else if ("46000".equals(operator) || "46002".equals(operator) || "46004".equals(operator) || "46007".equals(operator)) {
            opeType = "中国移动";
        } else if ("46003".equals(operator) || "46005".equals(operator) || "46011".equals(operator)) {
            opeType = "中国电信";
        }
        return opeType;
    }

    /**
     * 获取IMEI 需要动态申请权限 READ_PHONE_STATE
     */
    @SuppressLint("MissingPermission")
    public static String getIMEI(Context context) {
        TelephonyManager telephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
        if (telephonyManager == null) {
            return "";
        }
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                return telephonyManager.getImei();
            } else {
                return telephonyManager.getDeviceId();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }
}
