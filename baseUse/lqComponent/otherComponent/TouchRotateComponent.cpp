//
//  TouchRotateComponent.cpp
//  UnicornDonut
//
//  Created by luotianqiang on 17/9/1.
//
//

#include "TouchRotateComponent.h"
IMPLEMENT_COCOS2DX_CLASS(TouchRotateComponent);
 std::string TouchRotateComponent::TouchRoateteEnd="TouchRoateteEnd";
bool TouchRotateComponent::init(){
    if(TouchNodeComponent::init()){
        _endRotate = 180;
        this->addListners(ComponentTouchBegan, [this](LQComponent*,OperateListner*){
            touchPrePos = Vec2::ZERO;
        });
          this->addListners(ComponentTouchMove, CC_CALLBACK_2(TouchRotateComponent::rotateTime, this));
        return true;
    }
    return false;
}

void TouchRotateComponent::rotateTime(LQComponent* lc,OperateListner* lis){
    if(touchPrePos != Vec2::ZERO){
        auto currentPos = dynAny_cast<Vec2>(lis->getData());
        
        auto time = lc->getOwner();
        auto size = time->getContentSize();
        auto touchPos = dynAny_cast<Vec2>(lis->getData());
        
        auto centerPos = time->convertToWorldSpace(Vec2(size.width*time->getAnchorPoint().x,size.height*time->getAnchorPoint().y));
        auto endDirect = touchPos - centerPos;
        auto preDirect = touchPrePos - centerPos;
        auto angel = endDirect.getAngle(preDirect)*180/3.1415926f;
        
////        if(abs(angel)>135)
////            angel = 0;
        auto newRotate = time->getRotation()+angel;
////        if(newRotate<0)
////            newRotate = 0;
//        time->setRotation(newRotate);
//        this->dispatcherEvent(TouchRoateteEnd);
////        if(newRotate>_endRotate){
////            time->setRotation(_endRotate);
////            this->dispatcherEvent(TouchRoateteEnd);
////        }
        log("%f", newRotate);
        if(!_isSwitch){
        
            auto newRotate = time->getRotation()+angel;
            time->setRotation(newRotate);
            this->dispatcherEvent(TouchRoateteEnd);
            
        }else{
            if(newRotate < 0)
                newRotate = 0;
            if(newRotate >= _endRotate){
                time->setRotation(_endRotate);
                this->dispatcherEvent(TouchRoateteEnd);
            }else{
                time->setRotation(newRotate);
            }
        }
        
        
        
    }
    touchPrePos = dynAny_cast<Vec2>(lis->getData());
}

