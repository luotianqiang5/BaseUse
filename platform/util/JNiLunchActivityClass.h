//
//  JNiLunchActivityClass.hpp
//  makeburrito
//
//  Created by luotianqiang1 on 15/11/30.
//
//

#ifndef JNiLunchActivityClass_hpp
#define JNiLunchActivityClass_hpp

#include <iostream>
#include "cocos2d.h"
USING_NS_CC;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>

using namespace std;

class JNiLunchActivityClass{
public:
    static  jobject getContext();
    static jclass getClass();
    int static callIntFun(string _methodName);
    static void  destroy();
private:
    static const char* ACTIVITY_CLASSName;
    static jobject context;
    static jclass _launchActivityClass;
};


#endif /* JNiLunchActivityClass_hpp */
#endif