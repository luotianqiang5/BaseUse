//
//  BaseDecorationLayer.cpp
//  makeburrito
//
//  Created by luotianqiang1 on 15/11/19.
//
//

#include "BaseDecorationLayer.h"
#include "MyDialog.h"
#include "TouchScaleRoteComponent.h"
#include "CategoryReader.h"
#include "BaseShopLayer.h"
#include "RewardManager.h"
USING_NS_KD;


const string BaseDecorationLayer:: _lockName = "_lockName";

BaseDecorationLayer::BaseDecorationLayer():
_showIconAciton("showIcon")
,_hideIconAction("hideIcon")
,resetName("btn_reset")
,resetMessage("Are you sure you want to reset?")
,_decoView(nullptr)
,_lockImage("lock.png")
,_rewardLock("ui/getfree.png")
,_currentcategory("")
,shopClassName("ShopLayer")
,_chooseBg(nullptr)
,tempIndex(-1)
,tempKey("")
,moduleName(""){
      lock_postion_type = TYPE_LEFT_BOTTOM;
}

BaseDecorationLayer::~BaseDecorationLayer(){
    CC_SAFE_RELEASE_NULL(_chooseBg);
}

void BaseDecorationLayer::touchEnd(ui::Widget* widget){
    if(resetName.compare(widget->getName()) == 0){
        auto _dailog = MyDialog::create(resetMessage, MyDialog::twoBtnPath);
        _dailog->dialogBtnClick = [&](MyDialog * dialog,int index){
            if(index == 0)
                reset();
        };
        getScene()->addChild(_dailog);
    }
}

void BaseDecorationLayer::creatIcon(size_t _index){
    if(_chooseBg != nullptr)
        _chooseBg->removeFromParent();
    _currentcategory = categoryNames.at(_index);
    if(_decoView != nullptr){
        auto _adapter = createAdapter(_currentcategory);
        _decoView->changeAdapter(_adapter);
        
        for(int i=0;i<_decoView->nodeCache.size();i++){
            auto addLockFunc = [=](){
                auto _node = _decoView->nodeCache.at(i);
                auto _lock = Sprite::create(_lockImage);
                _lock->setName(_lockName);
                if(lock_postion_type == TYPE_CENTER){
                    _lock->setPosition(_node->getContentSize()*.5);
                }else {
                    _lock->setAnchorPoint(Vec2(1,0));
                    _lock->setPosition(_node->getContentSize().width, 0);
                }
                _node->addChild(_lock);
            };
            if(!isFree(_currentcategory,i+1)){
                addLockFunc();
            }else{
                auto moduleNameTemp = moduleName;
                if(moduleNameTemp.empty())
                    moduleNameTemp = _currentcategory;
                auto rewardInfo = kRewardManager->getRewardInfoItem(moduleNameTemp, _currentcategory, i+1);
                auto key = rewardInfo.getKey();
                if(kRewardManager->isLocked(key)&&!IAPManager::getInstance()->isPackagePurchased(rewardInfo.iapId)){
                    //#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
                    //                    if(!_cfsys.checkNetworkAvailable())
                    //                        addLockFunc();
                    //                    else
                    //#endif
                    {
                        auto _node = _decoView->nodeCache.at(i);
                        auto _lock = Sprite::create(_rewardLock);
                        _lock->setName(_lockName);
                        if(lock_postion_type == TYPE_CENTER){
                            _lock->setPosition(_node->getContentSize()*.5);
                        }else {
                            _lock->setAnchorPoint(Vec2(1,0));
                            _lock->setPosition(_node->getContentSize().width, 0);
                        }
                        _node->addChild(_lock);
                    }
                }
            }
        }
        if(_decoView->getDirection() ==MyScollView::Direction::VERTICAL)
            _decoView->getContainer()->setPosition(  _decoView->getContainer()->getPosition()+Vec2(0,400));
        else
            _decoView->getContainer()->setPosition(  _decoView->getContainer()->getPosition()+Vec2(-400,0));
        _adapter->setItemClick(CC_CALLBACK_3(BaseDecorationLayer::catagoreClick,this));
    }
    
}

bool BaseDecorationLayer::isFree(const string& s,size_t index){
    return IAPManager::getInstance()->isFree(s, index);
}

vector<string> BaseDecorationLayer::getIconPath(const string& _currentcategory){
    return CategoryReader::getIconPath(_currentcategory);
}

void BaseDecorationLayer::showCatagore(){
    if(_operate != nullptr)
        _operate->playAction(_showIconAciton,[&](){
            FiniteTimeAction* moveAction;
            if(_decoView->getDirection() ==MyScollView::Direction::VERTICAL)
                moveAction = MoveBy::create(0.5, Vec2(0,-400));
            else
                moveAction = MoveBy::create(0.5, Vec2(400,0));
            _decoView->getContainer()->runAction(Sequence::create(moveAction, CallFunc::create([&](){
                for(int i=0;i<_decoView->nodeCache.size();i++){
                    auto _node = _decoView->nodeCache.at(i);
                    _node->runAction(ActionHelper::getScaleBtnAction());
                }
            }), nullptr));
            this->showEnd();
        });
}

void BaseDecorationLayer::hideCatagore(){
    if(_operate != nullptr)
        _operate->playAction(_hideIconAction,CC_CALLBACK_0(BaseDecorationLayer::hideEnd, this));
}

void BaseDecorationLayer::catagoreClick(EventSprite* s,size_t _index,MyScollView* scol) {
    auto moduleNameTemp = moduleName;
    if(moduleNameTemp.empty())
        moduleNameTemp = _currentcategory;
    auto rewardInfo = kRewardManager->getRewardInfoItem(moduleNameTemp, _currentcategory, _index+1);
     auto key = rewardInfo.getKey();
    if(kRewardManager->isLocked(key)&&!IAPManager::getInstance()->isPackagePurchased(rewardInfo.iapId)){
        //#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        //        if(!_cfsys.checkNetworkAvailable())
        //            gotoShop();
        //        else
        //#endif
        {
            kRewardManager->showRewardAds(key);
            tempKey = key;
            tempIndex = _index;
        }
    }else if(isFree(_currentcategory,_index+1)){
        if(_chooseBg != nullptr){
            _chooseBg->removeFromParent();
            _chooseBg->setPosition(s->getContentSize()*.5);
            s->addChild(_chooseBg,-1);
        }
        SoundPlayer::getInstance()->playEffect("sound/other/category_selection_1.mp3");
        freeClick(s,_index,scol);
    } else
        gotoShop();
    
}

void BaseDecorationLayer::gotoShop(){
    auto _shopLayer = dynamic_cast<BaseShopLayer*>(DynObject::createDynObject(shopClassName));
    if(_shopLayer != nullptr){
        
        _shopLayer->_callBack = [&](){
            auto pos = _decoView->getContainer()->getPosition();
            for(int i=0;i<categoryNames.size();i++){
                if(categoryNames.at(i) == _currentcategory){
                    this->creatIcon(i);
                    _decoView->getContainer()->setPosition(pos);
                }
            }
        };
        getScene()->addChild(_shopLayer);
    }
}

void BaseDecorationLayer::hideUi(Node* _root){
    if(_root != nullptr){
        auto _com = dynamic_cast<TouchScaleRoteComponent*>(_root->getComponent("TouchScaleRoteComponent"));
        if(_com != nullptr)
            _com->hideUI();
        for(auto _child:_root->getChildren())
            hideUi(_child);
    }
}

void BaseDecorationLayer::saveImage(Node* _node,Size _size,string _name){
    if(_node != nullptr) {
        auto filePath = FileUtils::getInstance()->getWritablePath() + _name;
        filePath = FileUtils::getInstance()->fullPathForFilename(filePath);
        Director::getInstance()->getTextureCache()->removeTextureForKey(filePath);
        std::remove(filePath.c_str());
        hideUi(_node);
        auto orgPos = _node->getPosition();
        auto orgAr = _node->getAnchorPoint();
        _node->setPosition(Vec2::ZERO);
        _node->setAnchorPoint(Vec2::ZERO);
        auto _render = RenderTexture::create(_size.width, _size.height, Texture2D::PixelFormat::RGBA8888, GL_DEPTH24_STENCIL8);
        _render->begin();
        _node->visit();
        _render->end();
        Director::getInstance()->getRenderer()->render();
        _node->setPosition(orgPos);
        _node->setAnchorPoint(orgAr);
        _render->saveToFile(_name);
        Director::getInstance()->getRenderer()->render();
    }
}

void BaseDecorationLayer::onEnterTransitionDidFinish() {
    BaseStepLayer::onEnterTransitionDidFinish();
    if(_operate != nullptr){
        auto _next = _operate->getNodeByName("btn_next");
        if(_next != nullptr)
            _next->stopAllActions();
        
    }
    kRewardManager->showDialog = [this](){
        auto dialog = MyDialog::create("Rewarded Ads is currently unavailable", "RewardDialog.csb");
        dialog->dialogBtnClick = [this](MyDialog*,int index){
            if(index == 0){
                this->gotoShop();
            }
        };
        getScene()->addChild(dialog);
    };
    kRewardManager->showRewardFalseCall = [this](){
        tempIndex = -1;
        tempKey = "";
    };
    kRewardManager->removeRewardLoadingCall = [this](){
        if(tempIndex != -1&&!kRewardManager->isLocked(tempKey)){
            auto tempNode = _decoView->nodeCache.at(tempIndex)->getChildByName(_lockName);
            if(tempNode){
                tempNode->retain();
                tempNode->removeFromParent();
                auto pos = _decoView->getContainer()->getPosition();
                for(int i=0;i<categoryNames.size();i++){
                    if(categoryNames.at(i) == _currentcategory){
                        this->creatIcon(i);
                        _decoView->getContainer()->setPosition(pos);
                    }
                }
                _decoView->nodeCache.at(tempIndex)->addChild(tempNode);
                tempNode->release();
                tempNode->runAction(Sequence::create(FadeOut::create(0.5),RemoveSelf::create(), nullptr));
                ActionHelper::delayFunc(0.4, tempNode, [tempNode](){
                    if( tempNode->getParent()){
                        auto particle = ParticleSystemQuad::create("particles/fallStarParticle.plist");
                        particle->setPosition(tempNode->getParent()->getContentSize()*.5);
                        tempNode->getParent()->addChild(particle);
                        particle->stopSystem();
                        
                        SoundPlayer::getInstance()->playEffect("sound/general/draw_success.mp3");
                        particle->resetSystem();
                    }
                });
            }
        }
        tempIndex = -1;
        tempKey = "";
    };
}

void BaseDecorationLayer::onExitTransitionDidStart(){
    kRewardManager->showRewardFalseCall = nullptr;
    kRewardManager->removeRewardLoadingCall = nullptr;
    kRewardManager->showDialog = nullptr;
    BaseStepLayer::onExitTransitionDidStart();
}

void BaseDecorationLayer::stopPrompt(){
    BaseStepLayer::stopPrompt();
    auto down = _operate->getNodeByName("down");
    if(down != nullptr) {
        down->stopAllActions();
        down->setVisible(false);
    }
}
MyBaseAdapter* BaseDecorationLayer::createAdapter(const string& _currentcategory){
    return MyBaseAdapter::create(getIconPath(_currentcategory));
}
