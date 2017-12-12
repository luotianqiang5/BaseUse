//
//  DeviceInfo.cpp
//  WeddingFoodMaker
//
//  Created by luotianqiang on 16/4/7.
//
//

#include "DeviceInfo.h"
#include "cocos2d.h"
USING_NS_CC;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <math.h>
#include <sys/utsname.h>
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include "JNiLunchActivityClass.h"
#define JAVAENV JniHelper::getEnv()
#define CLASS_NAME "com/make/base/DeviceInfo"

MachineInfo DeviceInfo::_machineInfo = {"",0,""};


float DeviceInfo::getDiagonal(){
    return getMachineInfo().diagonal;
}

MachineInfo DeviceInfo::getMachineInfo(){
    if(_machineInfo.key.empty()) {
        struct utsname systemInfo;
        uname(&systemInfo);
        _machineInfo.key = systemInfo.machine;
        _machineInfo.name = systemInfo.sysname;
        jobject context = JNiLunchActivityClass::getContext();
        if(NULL != context){
            
            jmethodID getResourcesId = JAVAENV->GetMethodID(JNiLunchActivityClass::getClass(),"getResources","()Landroid/content/res/Resources;");
            jobject reource = JAVAENV->CallObjectMethod(context,getResourcesId);
            
            jclass resourcesClass = JAVAENV->FindClass("android/content/res/Resources");
            jmethodID getDisplayMetricsID = JAVAENV->GetMethodID(resourcesClass,"getDisplayMetrics","()Landroid/util/DisplayMetrics;");
            jobject displayMetrics = JAVAENV->CallObjectMethod(reource,getDisplayMetricsID);
            
            jclass displayMetricsClass = JAVAENV->FindClass("android/util/DisplayMetrics");
            jfieldID widthid = JAVAENV->GetFieldID(displayMetricsClass,"widthPixels","I");
            jfieldID heightid = JAVAENV->GetFieldID(displayMetricsClass,"heightPixels","I");
            jfieldID densityDpiid = JAVAENV->GetFieldID(displayMetricsClass,"densityDpi","I");
          
            float width = (int)(JAVAENV->GetIntField(displayMetrics,widthid));
            float height = (int)(JAVAENV->GetIntField(displayMetrics,heightid));
            float densityDpi = (int)(JAVAENV->GetIntField(displayMetrics,densityDpiid));
            _machineInfo.diagonal = std::sqrt(width*width+height*height)/densityDpi;
//            JAVAENV->DeleteGlobalRef(displayMetricsClass);
//            JAVAENV->DeleteGlobalRef(displayMetrics);
//            JAVAENV->DeleteGlobalRef(reource);
//            JAVAENV->DeleteGlobalRef(resourcesClass);
            
        }
        JNiLunchActivityClass::destroy();
        
        
        
//        JniMethodInfo t;
//        if(JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getDiagonal", "()F"))
//        {
//            jfloat f = t.env->CallStaticFloatMethod(t.classID, t.methodID);
//            _machineInfo.diagonal = (float)f;
//        }
    }
    return _machineInfo;
}
#endif