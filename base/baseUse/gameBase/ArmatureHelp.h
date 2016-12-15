//
//  ArmatureHelp.h
//  IceCreamSundae
//
//  Created by luotianqiang1 on 16/5/25.
//
//

#ifndef ArmatureHelp_hpp
#define ArmatureHelp_hpp

#include <iostream>
#include "cocos2d.h"
#include "cocostudio/cocostudio.h"
#include <spine/spine-cocos2dx.h>
#include "spine/spine.h"
class ArmatureHelp {
public:
    static void changeSpriteFrame(cocostudio::Armature*,const std::string& searchPath);
    static cocos2d::SpriteFrame* getSpriteFrame(const std::string& _boneName,const std::string& searchPath);//根据骨骼名称搜索图片
    static void randAnimation(cocostudio::Armature*,const std::vector<std::string>& names,float delayTime);
    static void randAnimation(spine::SkeletonAnimation*,const std::vector<std::string>& names,float delayTime);
};
#endif /* ArmatureHelp_hpp */
