package com.lucky.devicelib;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;

/**
 * @author : 杜宗宁
 * @date : 2018/9/7
 * 收集cpu相关信息工具类
 */
public class CpuUtils {
    private static final String CPU_MAX_FREQ = "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq";
    private static final String CPU_MIN_FREQ = "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq";
    private static final String CPU_CUR_FREQ = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";
    private static final String CPU_NAME = "/proc/cpuinfo";

    /**
     * 获取CPU最大频率（单位KHZ）
     * "/system/bin/cat" 命令行
     * "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq" 存储最大频率的文件的路径
     */
    public static String getMaxCpuFreq() {
        return getCpuFreq(CPU_MAX_FREQ);
    }

    /**
     * 获取CPU最小频率（单位KHZ）
     */
    public static String getMinCpuFreq() {
        return getCpuFreq(CPU_MIN_FREQ);
    }

    private static String getCpuFreq(String text) {
        StringBuilder result = new StringBuilder();
        InputStream in = null;
        try {
            String[] args = {"/system/bin/cat", text};
            ProcessBuilder cmd = new ProcessBuilder(args);
            Process process = cmd.start();
            in = process.getInputStream();
            byte[] re = new byte[24];
            while (in.read(re) != -1) {
                result.append(new String(re));
            }
        } catch (IOException ex) {
            ex.printStackTrace();
            result = new StringBuilder("N/A");
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return result.toString().trim();
    }

    /**
     * 实时获取CPU频率
     */
    public static String getCurCpuFreq() {
        String result = "N/A";
        BufferedReader br = null;
        try {
            FileReader fr = new FileReader(CPU_CUR_FREQ);
            br = new BufferedReader(fr);
            String text = br.readLine();
            result = text.trim();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
        return result;
    }

    /**
     * 获取CPU名字
     */
    public static String getCpuName() {
        BufferedReader br = null;
        try {
            FileReader fr = new FileReader(CPU_NAME);
            br = new BufferedReader(fr);
            String text = br.readLine();
            String[] array = text.split(":\\s+", 2);
            return array[1];
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
        return "N/A";
    }
}
