//
//  BaseMakeLayer.hpp
//  DeepFryAnythin
//
//  Created by luotianqiang1 on 16/1/21.
//
//

#ifndef BaseMakeLayer_hpp
#define BaseMakeLayer_hpp

#include "BaseStepLayer.h"
#include "MoveHideShade.h"
#include "DragFall.h"
#include "TimerChangeComponent.h"
class BaseMakeLayer: public BaseStepLayer{
    CC_SYNTHESIZE_RETAIN(ActionInterval*, _mixAction, MixAction);
protected:
     BaseMakeLayer();
    virtual ~BaseMakeLayer();
    virtual void onEnterTransitionDidFinish();
    virtual void onEnter();
    void showNode(const string&,ActionHelper::ShowDirection,float delayTime = 0);
    void touchEnd(ui::Widget* widget);
    virtual void mixEnd(LQComponent*, OperateListner* _lis);
    virtual void startFall(LQComponent*, OperateListner* _lis);
    virtual void fallEnd(LQComponent*, OperateListner* _lis);
    virtual void moveToTarget(LQComponent*, OperateListner* _lis);
   
    virtual void showSpoon();
    void layerScale();
    void layerScaleBack();
    void showBowl();
     virtual void stopPrompt();
    void showAddPrompt();
    void hideThing(Node* node,float delayTime = 0);
    void changeNodeToClipping(const vector<std::string>& nodeNames);
protected:
    MoveHideShade* moveSpoon;
    TimerChangeComponent *mix;
    void scaleBowl();
    void scaleBackBowl();
    DragFall* createDraOperate(Node* owner = nullptr);
protected:
    Vec2 spoonCheckPoint;
    float spoonRote;
    vector<string> mixPath;
    bool isPouring;
    int count,addEndNum;
    Node* gravityPrompt;
    NodeGrid* grid;
};


#endif /* BaseMakeLayer_hpp */
