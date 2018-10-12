//
// Created by Admin on 2018/9/25.
//

#ifndef MONITORNATIVE_EMULATOR_H
#define MONITORNATIVE_EMULATOR_H


//----------------------------模拟器检测 start------------------------

#include <jni.h>

bool detector_file_exists(const char *);
bool is_special_emulator();
bool check_contain_bluetooth(JNIEnv *);
bool check_contain_cpu_Temperature();
bool check_contain_BaseBand(JNIEnv *);
std::string detectIsEmulator(JNIEnv *env,std::string result);

#endif //MONITORNATIVE_EMULATOR_H
