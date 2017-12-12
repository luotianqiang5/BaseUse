#include "BaseStepLayer.h"
#include "KeyBackEvent.h"
using namespace cocos2d::ui;

std::function<bool()> BaseStepLayer::keyBack = nullptr;
unordered_map<string, function<void(ui::Widget*)>> * BaseStepLayer::clickBtns = nullptr;
bool BaseStepLayer::onBack(){
    if(keyBack != nullptr)
        return keyBack();
    return false;
}

BaseStepLayer::BaseStepLayer(){
    _loopSound = -1;
    _operate = nullptr;
    autoShowBtnName = {"btn_home","btn_more"};
}
BaseStepLayer::~BaseStepLayer(){
    if(_loopSound != -1) {
        SoundPlayer::getInstance()->stopEffectLoop(_loopSound);
        _loopSound = -1;
    }
}

bool BaseStepLayer::init(){
    if(Layer::init()) {
        _prompt = _operate->getNodeByName<Sprite*>("finger");
        if(!_prompt){
            _prompt = Sprite::create("hand.png");
            if(_prompt){
                _prompt->setVisible(false);
                addChild(_prompt,300);
            }
        }
        return true;
    }
    return false;
}

void BaseStepLayer::stopPrompt(){
    if(_prompt != nullptr){
        _prompt->stopAllActions();
        _prompt->setVisible(false);
        _prompt->setOpacity(255);
        _prompt->setRotation(0);
    }
}

void BaseStepLayer::onEnterTransitionDidFinish() {
    Layer::onEnterTransitionDidFinish();
    Director::getInstance()->getTextureCache()->removeUnusedTextures();
    
    KeyBackEvent::getInstance()->addBackEvent(std::bind(&BaseStepLayer::onBack, this), this);
    if(_operate != nullptr && _operate->_touchBtnCallback == nullptr){
        _operate->_touchBtnCallback = [this](ui::Widget* widget){
            bool hasEvent = false;
            if(clickBtns != nullptr){
                auto itor = clickBtns->find(widget->getName());
                if(itor != clickBtns->end()){
                    hasEvent = true;
                    itor->second(widget);
                }
            }
            if(!hasEvent)
                touchEnd(widget);
        };
    }
    if(_operate != nullptr){
        
        auto visibleSize = Director::getInstance()->getVisibleSize();
        float delayTime = 0;
        for(auto index =0;index<autoShowBtnName.size();index++){
            auto btn = _operate->getNodeByName(autoShowBtnName[index]);
            if(btn){
                
                auto dir = ActionHelper::ShowDirection::show_from_left;
                auto btnWordPos = btn->getParent()->convertToWorldSpace(btn->getPosition());
                if(btnWordPos.x>visibleSize.width*.5)
                    dir = ActionHelper::ShowDirection::show_from_right;
                ActionHelper::delayFunc(delayTime, btn, [btn,dir](){
                    ActionHelper::showBackInOut(btn, btn->getPosition(), dir);
                    btn->setVisible(true);
                });
                delayTime+=0.15f;
            }
        }
        
        auto btn_next_light = _operate->getNodeByName("btn_next_light");
        if(btn_next_light != nullptr){
            btn_next_light->stopAllActions();
            btn_next_light->runAction(RepeatForever::create(RotateBy::create(1, 90)));
        }
        
        auto _next = _operate->getNodeByName("btn_next");
        if(_next != nullptr){
            _next->stopAllActions();
            _next->setRotation(0);
            _next->setScale(1);
            _next->runAction(RepeatForever::create(Sequence::create(DelayTime::create(2),Sequence::create(EaseExponentialOut::create(Sequence::create(ScaleTo::create(0.65, 1.1),ScaleTo::create(0.5, 1),nullptr)), nullptr), nullptr)));
            //            _next->runAction(RepeatForever::create(Sequence::create(DelayTime::create(2),Spawn::create(JumpBy::create(1, Vec2::ZERO, 15, 2),RotateBy::create(1, 360), nullptr), nullptr)));
        }
    }
}

void  BaseStepLayer::onEnter(){
    Layer::onEnter();
    changeHDPic("bg");
    if(_operate != nullptr){
        
        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto bg = _operate->getNodeByName("bg");
        if(bg){
            if(bg->getContentSize().height<visibleSize.height){
                bg->setScale(visibleSize.height/bg->getContentSize().height);
            }
            if(bg->getContentSize().width<visibleSize.width)
                  bg->setScale(visibleSize.width/bg->getContentSize().width);
        }
        for(auto name:autoShowBtnName){
            auto btn = _operate->getNodeByName(name);
            if(btn){
                btn->setVisible(false);
            }
        }
    }
}

void BaseStepLayer::touchEnd(ui::Widget* widget) {
    
}

void BaseStepLayer::changeHDPic(const string& nodeName,const string& suffix){
    do {
        auto size = Director::getInstance()->getOpenGLView()->getFrameSize();
        auto max = std::max(size.height, size.width);
        CC_BREAK_IF(max<2048);
        CC_BREAK_IF(!_operate);
        auto bg = _operate->getNodeByName<Sprite*>(nodeName);
        CC_BREAK_IF(!bg);
        auto filePath =   Director::getInstance()->getTextureCache()->getTextureFilePath(bg->getTexture());
        CC_BREAK_IF(filePath.empty());
        auto  str =  filePath.insert(filePath.find_last_of("."), suffix);
        CC_BREAK_IF(!FileUtils::getInstance()->isFileExist(str));
        auto s = Sprite::create(str);
        s->setScale(0.5);
        s->setPosition(bg->getContentSize()*.5);
        bg->addChild(s);
    }while (false);
}

void BaseStepLayer::insertBtnClick(const string& btn_name,function<void(ui::Widget*)> click_event){
    if(click_event != nullptr){
        if(clickBtns == nullptr)
            clickBtns = new unordered_map<string, function<void(ui::Widget*)>>();
        (*clickBtns)[btn_name] = click_event;
    }
}