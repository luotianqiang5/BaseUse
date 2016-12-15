//
//  ArmatureHelp.cpp
//  IceCreamSundae
//
//  Created by luotianqiang1 on 16/5/25.
//
//

#include "ArmatureHelp.h"
using namespace std;
using namespace cocos2d;
using namespace cocostudio;
using namespace spine;
void ArmatureHelp::changeSpriteFrame(cocostudio::Armature* _armature,const string& searchPath){
    auto _boneDic = _armature->getBoneDic();
    for(auto _child:_boneDic.keys()) {
        auto _list = _boneDic.at(_child)->getDisplayManager()->getDecorativeDisplayList();
        for(auto _node:_list){
            auto  _name = string(_node->getDisplayData()->displayName.c_str());
            const char* temp = strtok((char*)_name.c_str(), ".");
            auto _tempSpriteFrame = getSpriteFrame(temp,searchPath);
            if(_tempSpriteFrame != nullptr){
                auto _skin = dynamic_cast<Skin*>(_node->getDisplay());
                if(nullptr != _skin){
                    _skin->setSpriteFrame(_tempSpriteFrame);
                }
            } else {
                CCLOG((searchPath+"/"+temp+".png not found").c_str());
                auto _skin = dynamic_cast<Skin*>(_node->getDisplay());
                if(nullptr != _skin){
                    _skin->setSpriteFrame(Sprite::create()->getSpriteFrame());
                }
            }
        }
    }

}

cocos2d::SpriteFrame* ArmatureHelp::getSpriteFrame(const string& _boneName,const string& searchPath){
    auto path = searchPath+"/"+_boneName+".png";
    auto s = SpriteFrameCache::getInstance()->getSpriteFrameByName(path);
    if(s == nullptr) {
        auto tex = Director::getInstance()->getTextureCache()->getTextureForKey(path);
        if(tex != nullptr)
            s = Sprite::createWithTexture(tex)->getSpriteFrame();
    }
    if(s == nullptr) {
        auto itor = _boneName.find("/");
        if(itor != string::npos) {
            s = getSpriteFrame(string(_boneName).erase(0, itor+1), searchPath);
        }
    }
    return s;
}

void ArmatureHelp::randAnimation(cocostudio::Armature* node,const std::vector<std::string>& names,float delayTime){
    const int actionTag = 10101010;
    node->stopActionByTag(actionTag);
    auto action = RepeatForever::create(Sequence::create(CallFunc::create([=](){
        node->getAnimation()->play(names.at(rand()%names.size()),-1,0);
    }), DelayTime::create(delayTime), nullptr));
    action->setTag(actionTag);
    node->runAction(action);
}

void ArmatureHelp::randAnimation(spine::SkeletonAnimation* node,const std::vector<std::string>& names,float delayTime){
    const int actionTag = 10101010;
    node->stopActionByTag(actionTag);
    auto action = RepeatForever::create(Sequence::create(CallFunc::create([=](){
        node->addAnimation(0, names.at(rand()%names.size()), false);
    }), DelayTime::create(delayTime), nullptr));
    action->setTag(actionTag);
    node->runAction(action);
}