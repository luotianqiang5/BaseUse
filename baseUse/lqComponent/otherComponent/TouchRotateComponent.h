//
//  TouchRotateComponent.h
//  UnicornDonut
//
//  Created by luotianqiang on 17/9/1.
//
//

#ifndef TouchRotateComponent_hpp
#define TouchRotateComponent_hpp

#include "TouchNodeComponent.h"
class TouchRotateComponent: public TouchNodeComponent{
    CC_SYNTHESIZE(float, _endRotate, EndRotate);
    CC_SYNTHESIZE(bool, _isSwitch, IsSwitch);
    DECLARE_COMPONENT_DYNCLASS(TouchRotateComponent);
public:
     virtual bool init();
protected:
    Vec2 touchPrePos;
    void rotateTime(LQComponent* lc,OperateListner* lis);
public:
    static std::string TouchRoateteEnd;
};

#endif /* TouchRotateComponent_hpp */
