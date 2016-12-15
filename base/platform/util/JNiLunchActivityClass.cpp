//
//  JNiLunchActivityClass.cpp
//  makeburrito
//
//  Created by luotianqiang1 on 15/11/30.
//
//

#include "JNiLunchActivityClass.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define JAVAVM JniHelper::getJavaVM()
#define JAVAENV JniHelper::getEnv()

 const char* JNiLunchActivityClass::ACTIVITY_CLASSName = "com/common/android/LaunchActivity";

 jobject JNiLunchActivityClass::context = nullptr;
 jclass JNiLunchActivityClass::_launchActivityClass = nullptr;



jobject JNiLunchActivityClass::getContext(){
    if(context == nullptr){
        _launchActivityClass =  JAVAENV->FindClass(ACTIVITY_CLASSName);
        jfieldID contextID = JAVAENV->GetStaticFieldID(_launchActivityClass,"actInstance","Landroid/app/Activity;");
        if(contextID==NULL)
            log("actInstance ===== field not foud");
        context = JAVAENV->GetStaticObjectField(_launchActivityClass,contextID);
    }
    return context;
}

jclass JNiLunchActivityClass::getClass(){
    return _launchActivityClass;
}

int JNiLunchActivityClass::callIntFun(string _methodName){

    getContext();
    jmethodID getPlatFom =  JAVAENV->GetMethodID(_launchActivityClass,_methodName.c_str(),"()I");

    jint plateForm = JAVAENV->CallIntMethod(context,getPlatFom);

   return (int)plateForm;
}

void JNiLunchActivityClass::destroy(){
//    if(context != nullptr)
//        JAVAENV->DeleteGlobalRef(context);
//    context = nullptr;
//    if(_launchActivityClass != nullptr)
//        JAVAENV->DeleteGlobalRef(_launchActivityClass);
//    _launchActivityClass = nullptr;
}
#endif
