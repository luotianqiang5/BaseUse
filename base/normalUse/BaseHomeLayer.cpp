//
//  BaseHomeLayer.cpp
//  makeburrito
//
//  Created by luotianqiang1 on 15/11/23.
//
//

#include "BaseHomeLayer.h"
#include "MyDialog.h"
#include "AdLoadingLayerBase.h"
#include "BaseShopLayer.h"
BaseHomeLayer::BaseHomeLayer():
_shopClass("ShopLayer")
,startClass("")
{
    autoShowBtnName.clear();
    startEffect = "sound/start.mp3";
    privacyUrl = "https://www.crazycampmedia.com/privacys/";
}

bool BaseHomeLayer::onBack(){
    auto dialog = MyDialog::create("Are you sure you want to exit this game?", MyDialog::twoBtnPath);
    dialog->dialogBtnClick = [&](MyDialog * dialog,int index){
        if(index == 0) {
            Director::getInstance()->getEventDispatcher()->setEnabled(false);
            this->runAction(Sequence::create(DelayTime::create(0.5f),CallFunc::create([]{
                Director::getInstance()->end();
            }),nullptr));
        }
    };
    getScene()->addChild(dialog);
    return true;
}

void BaseHomeLayer::touchEnd(ui::Widget* widget){
    if(widget->getName().compare("btn_more") == 0){
        _cfsys.showMoreGame();
    }
    else if(widget->getName().compare("btn_shop") == 0){
        auto _shop = dynamic_cast<BaseShopLayer*>(DynObject::createDynObject(_shopClass));
        if(_shop != nullptr){
           addChild(_shop);
        }
    } else if(widget->getName().compare("btn_play") == 0)
        playClick();
    else if(widget->getName().compare("btn_rate")==0){
     //   _cfsys.rateUs();
    } else if(widget->getName() == "btn_privacy"){
        Application::getInstance()->openURL(privacyUrl);
    }
}

 void BaseHomeLayer::playClick(){
     auto _nextStep = dynamic_cast<BaseStepLayer*>(DynObject::createDynObject(startClass));
     Director::getInstance()->getEventDispatcher()->setEnabled(false);
     if(_nextStep != nullptr)
         _nextStep->retain();
     auto _callfunc = [_nextStep,this](){
         AdLoadingLayerBase::loadingDoneCallback = nullptr;
         SoundPlayer::getInstance()->playEffect(startEffect);
         if(_nextStep != nullptr){
             auto _scne = Scene::create();
             _scne->addChild(_nextStep);
             Director::getInstance()->replaceScene(TransitionFade::create(0.8, _scne, Color3B(255,165,228)));
             _nextStep->release();
         }
         if(IAPManager::getInstance()->isShowAds())
             AdsManager::getInstance()->showAds(ADS_TYPE::kTypeBannerAds);
         Director::getInstance()->getEventDispatcher()->setEnabled(true);
     };
     if(IAPManager::getInstance()->isShowAds()){
         AdLoadingLayerBase::loadingDoneCallback = _callfunc;
         showAdsCallBack();
     } else
         _callfunc();

}

void BaseHomeLayer::onEnterTransitionDidFinish(){
    BaseStepLayer::onEnterTransitionDidFinish();
}