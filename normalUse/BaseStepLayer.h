#ifndef coffee_BaseStepLayer
#define coffee_BaseStepLayer

#include"cocos2d.h"
#include "DynBase.h"
#include"IAPManager.h"
#include"AdsManager.h"
#include "cocostudio/cocostudio.h"
#include "ui/CocosGUI.h"
#include "SoundPlayer.h"
#include "StudioLayer.h"
#include "CocosHelper.h"
#include "ActionHelper.h"
#include "StudioLayer.h"
#include "MainScene.h"
#include "AudioPlay.h"
#include "STSystemFunction.h"
USING_NS_CC;
using namespace cocostudio;

class BaseStepLayer : public Layer,public DynObject
{
public:
    BaseStepLayer();
    virtual ~BaseStepLayer();
    StudioLayer * getOperateLayer(){return _operate;}
    static void setKeyBack(std::function<bool()> call){keyBack = call;}
    static void insertBtnClick(const string& btn_name,function<void(ui::Widget*)> click_event);
protected:
    virtual void touchEnd(ui::Widget* widget);
    virtual bool onBack();
    virtual bool init();
    virtual void onEnterTransitionDidFinish();
    virtual void onEnter();
    virtual void stopPrompt();
    void changeHDPic(const string& nodeName,const string& suffix="_h");
protected:
    StudioLayer *_operate;
    Sprite* _prompt;
    STSystemFunction _cfsys;
    int _loopSound;
    static std::function<bool()> keyBack;
    static unordered_map<string, function<void(ui::Widget*)>> *clickBtns;
    vector<string> autoShowBtnName;
};

#endif