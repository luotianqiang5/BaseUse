//
//  BaseMakeLayer.cpp
//  DeepFryAnythin
//
//  Created by luotianqiang1 on 16/1/21.
//
//

#include "BaseMakeLayer.h"
#include "DistanceToTarget.h"
#include "FrameStatus.h"
#include "TouchAccleRoteComponent.h"

BaseMakeLayer::BaseMakeLayer(){
    count =0;
    addEndNum = 0;
    spoonCheckPoint = Vec2(0.2,0.2);
    spoonRote=0;
    _mixAction = nullptr;
    mix = nullptr;
    isPouring = false;
    gravityPrompt = nullptr;
}

BaseMakeLayer::~BaseMakeLayer(){
    CC_SAFE_RELEASE_NULL(_mixAction);
}



void BaseMakeLayer::touchEnd(ui::Widget* widget){
    if(widget->getName().find("rote") != string::npos){
        widget->stopAllActions();
        widget->setRotation(0);
        widget->runAction(ActionHelper::createRoteAction(10,0.22));
    }
}

void BaseMakeLayer::mixEnd(LQComponent*, OperateListner* _lis){
    moveSpoon->setEnabled(false);
    string nodeNames[] = {"bowl","bowlUp"};
    for(int i=0;i<sizeof(nodeNames)/sizeof(*nodeNames);i++){
        auto node = _operate->getNodeByName(nodeNames[i]);
        if(node != nullptr){
            auto frame = FrameStatus::create(node->getName());
            frame->setSearchAction(_operate->getAction());
            auto action = frame->createAction(0.6, "addPos");
            if(action != nullptr)
                node->runAction(action);
        }
        
    }
    auto mixEndParticle = _operate->getNodeByName<ParticleSystemQuad*>("mixEndPraticle");
    if(mixEndParticle != nullptr)
        mixEndParticle->resetSystem();
    _operate->playNodeSound("mixEndPraticle");
    ActionHelper::hide(_operate->getNodeByName("spoon"), ActionHelper::ShowDirection::show_from_right);
}


void BaseMakeLayer::startFall(LQComponent* lc, OperateListner* _lis){
    auto drag = dynamic_cast<DragFall*>(lc);
    if(drag&& drag->getNeedGravity()&&gravityPrompt){
        gravityPrompt->stopAllActions();
        gravityPrompt->setVisible(false);
    }
}

void BaseMakeLayer::fallEnd(LQComponent*, OperateListner* _lis){
    //ActionHelper::delayFunc(1, this, CC_CALLBACK_0(BaseMakeLayer::layerScaleBack, this));
    // _eventDispatcher->setEnabled(true);
    isPouring = false;
    stopPrompt();
    count++;
    string nodeNames[] = {"bowl","bowlUp"};
    _operate->playNodeSound("bowl");
    for(int i=0;i<sizeof(nodeNames)/sizeof(*nodeNames);i++){
        auto scaleNode = _operate->getNodeByName(nodeNames[i]);
        if(scaleNode != nullptr)
            scaleNode->runAction(Sequence::create(ScaleTo::create(0.17, 1.05),ScaleTo::create(0.17, 1), nullptr));
    }
    if(count == addEndNum){
        ActionHelper::delayFunc(0.4, this, [this,nodeNames](){
            for(int i=0;i<sizeof(nodeNames)/sizeof(*nodeNames);i++){
                auto scaleNode = _operate->getNodeByName(nodeNames[i]);
                if(scaleNode != nullptr){
                    auto frame = FrameStatus::create(scaleNode->getName());
                    frame->setSearchAction(_operate->getAction());
                    auto action = frame->createAction(0.8, "mixPos");
                    if(action != nullptr)
                        scaleNode->runAction(action);
                }
            }
            showSpoon();
        });
    }
}

void BaseMakeLayer::moveToTarget(LQComponent*, OperateListner* _lis){
    if(!isPouring) {
        
        // _eventDispatcher->setEnabled(false);
        isPouring = true;
        layerScale();
        
    }
}

void BaseMakeLayer::showSpoon(){
    do{
        auto spoon = _operate->getNodeByName("spoon");
        CC_BREAK_IF(!spoon);
        if(mix == nullptr){
            mix = TimerChangeComponent::create();
            mix->setFrameFile(mixPath);
            mix->setDelayPerUnit(1.5);
            _operate->getNodeByName("mixLayer")->addComponent(mix);
            mix->addListners(FrameChangeeEnd,CC_CALLBACK_2(BaseMakeLayer::mixEnd,this));
        }
        SoundPlayer::getInstance()->playEffect("sound/general/ingredients_fly_in.mp3");
        ActionHelper::showBackInOut(spoon, spoon->getPosition(), ActionHelper::ShowDirection::show_from_right,[=](){
            auto mixPrompt = _operate->getNodeByName("mixPrompt");
            if(mixPrompt != nullptr){
                mixPrompt->stopAllActions();
                mixPrompt->setVisible(true);
                mixPrompt->runAction(RepeatForever::create(RotateBy::create(1, 90)));
            }
            DistanceToTarget *rote =DistanceToTarget::create();
            rote->setCheckPoint(spoonCheckPoint);
            rote->setTargetRect(CocosHelper::getNodeRectInWorld(_operate->getNodeByName("spoonLImit")));
            spoon->addComponent(rote);
            rote->addListners(MoveDistancePercent,[this](LQComponent* lq,OperateListner* _oper){
                float _percent = dynAny_cast<float>(_oper->getData());
                if(_percent<0)
                    _percent =0;
                else if(_percent>1)
                    _percent = 1;
                
                lq->getOwner()->setRotation(spoonRote*_percent);
            });
            
            
            moveSpoon = MoveHideShade::create();
            moveSpoon->setTarget(LQRect::create(CocosHelper::getNodeRectInWorld(_operate->getNodeByName("spoonLImit"))));
            moveSpoon->setCheckPoint(spoonCheckPoint);
            moveSpoon->setMoveType(MoveComponentType::kTypeBack);
            moveSpoon->addListners(ComponentTouchMove, [this,rote,spoon](LQComponent*, OperateListner* _lis){
                if(moveSpoon->checkInlimit()){
                    auto spoonMix = spoon->getChildByName("mixing");
                    if(spoonMix != nullptr){
                        spoon->setCascadeOpacityEnabled(false);
                        spoon->setOpacity(0);
                        spoonMix->setVisible(true);
                    }
                    auto mixing = _operate->getNodeByName("mixing");
                    if(mixing != nullptr)
                        mixing->setVisible(true);
                    auto mix0  = _operate->getNodeByName("mixing0");
                    if(mix0 != nullptr) {
                        mix0->setOpacity(0);
                        mix0->setVisible(true);
                        mix0->runAction(Sequence::create(DelayTime::create(0.2),FadeTo::create(0.2, 255),  nullptr));
                    }
                    rote->pauseUpdate();
                    spoon->runAction(RotateTo::create(0.2, spoonRote));
                    auto bowlUp = _operate->getNodeByName("bowlUp");
                    if(bowlUp != nullptr)
                        bowlUp->getParent()->reorderChild(bowlUp, 501);
                    _lis->setEnable(false);
                    moveSpoon->setBrintTop(false);
                    spoon->setZOrder(0);
                    moveSpoon->setMoveType(MoveComponentType::kTypeInRect);
                    moveSpoon->addListners(ComponentTouchMove, [this](LQComponent*, OperateListner* _lis){
                        moveSpoon->playSound();
                        mix->start();
                        if(_mixAction != nullptr){
                            const int actionTag = 2222;
                            if(mix->getOwner()->getActionByTag(actionTag) == nullptr){
                                _mixAction->setTag(actionTag);
                                mix->getOwner()->runAction(_mixAction);
                            }else
                                _actionManager->resumeTarget(mix->getOwner());
                        }
                        scaleBowl();
                        auto mixPrompt = _operate->getNodeByName("mixPrompt");
                        if(mixPrompt != nullptr){
                            mixPrompt->stopAllActions();
                            mixPrompt->setVisible(false);
                        }
                        
                    });
                    auto end = [rote,this](LQComponent*, OperateListner* _lis){
                        rote->pauseUpdate();
                        moveSpoon->stopSound();
                        mix->stop();
                        scaleBackBowl();
                        if(_mixAction != nullptr)
                            _actionManager->pauseTarget(mix->getOwner());
                    };
                    moveSpoon->addListners(ComponentTouchEnd,end);
                    moveSpoon->addListners(ComponentTouchCancle, end);
                } else {
                    rote->startUpdate();
                }
            });
            spoon->addComponent(moveSpoon);
        });
        spoon->setVisible(true);
    }while (false);
}

void BaseMakeLayer::onEnterTransitionDidFinish(){
    BaseStepLayer::onEnterTransitionDidFinish();
}

void BaseMakeLayer::onEnter() {
    BaseStepLayer::onEnter();
    gravityPrompt = _operate->getNodeByName("gravityPrompt");
    CocosHelper::visitAllNode(_operate, [](Node* node){
        auto _paritcle = dynamic_cast<ParticleSystemQuad*>(node);
        if(_paritcle != nullptr){
            _paritcle->setPositionType(ParticleSystem::PositionType::FREE);
            _paritcle->stopSystem();
        }
    });
}

void BaseMakeLayer::showNode(const string& nodeName,ActionHelper::ShowDirection dire,float delay) {
    auto _node = _operate->getNodeByName(nodeName);
    if(_node == nullptr)
        return;
    if(delay<0.0001){
        SoundPlayer::getInstance()->playEffect("sound/general/ingredients_fly_in.mp3");
        ActionHelper::showBezier(_node, _node->getPosition(), dire);
        _node->setVisible(true);
    }else {
        ActionHelper::delayFunc(delay, this, [this,dire,nodeName](){
            SoundPlayer::getInstance()->playEffect("sound/general/ingredients_fly_in.mp3");
            auto _node = _operate->getNodeByName(nodeName);
            ActionHelper::showBezier(_node, _node->getPosition(), dire);
            _node->setVisible(true);
        });
    }
}

void BaseMakeLayer::scaleBowl() {
    //    string nodeNames[] = {"bowl","bowl_up"};
    //    for(int i=0;i<2;i++){
    //        auto node = _operate->getNodeByName(nodeNames[i]);
    //        int actionTag =10;
    //        auto ac = node->getActionByTag(actionTag);
    //        if(ac == nullptr){
    //            ac = ScaleTo::create(0.6, 1.06);
    //            ac->setTag(actionTag);
    //            node->runAction(ac);
    //        }
    //    }
    
}
void BaseMakeLayer::scaleBackBowl(){
    //    string nodeNames[] = {"bowl","bowl_up"};
    //    for(int i=0;i<2;i++){
    //        auto node = _operate->getNodeByName(nodeNames[i]);
    //        node->stopAllActions();
    //        node->runAction(ScaleTo::create(0.6, 1));
    //    }
}

DragFall* BaseMakeLayer::createDraOperate(Node* owner){
    auto _con = DragFall::create();
    auto bowlUp = _operate->getNodeByName("bowlUp");
    if(bowlUp != nullptr){
        _con->addListners(FallThingstartFall, [bowlUp](LQComponent* lc,OperateListner* lis){
            lis->setEnable(false);
            lc->getOwner()->getParent()->reorderChild(lc->getOwner(), bowlUp->getLocalZOrder()-1);
        });
    }
    _con->addListners(FallThingfallEnd, CC_CALLBACK_2(BaseMakeLayer::fallEnd, this));
    _con->addListners(FallThingstartFall, CC_CALLBACK_2(BaseMakeLayer::startFall, this));
    auto rect = CocosHelper::getNodeRectInWorld(_operate->getNodeByName("limit"));
    _con->setTargetRect(rect);
    auto move = dynamic_cast<MoveHideShade*>(_con->getComponent("MoveHideShade"));
    move->addListners(ComponentTouchMove,[=](LQComponent*lq,OperateListner*ls){
        if(move->checkInlimit()){
            this->stopPrompt();
            if(_con->getNeedGravity()) {
                const int actionTag = 987;
                if(gravityPrompt != nullptr&& gravityPrompt->getActionByTag(actionTag) == nullptr){
                    auto accleRote = dynamic_cast<TouchAccleRoteComponent*>(_con->getComponent("TouchAccleRoteComponent"));
                    auto rote = accleRote->getEndRote();
                    auto rotaAction = -30;
                        gravityPrompt->setScaleX(1);
                    if(rote>0){
                        gravityPrompt->setScaleX(-1);
                        rotaAction = 30;
                    }
                    auto action = RepeatForever::create(Sequence::create(Show::create(), RotateTo::create(0, 0),RotateTo::create(0.5, rotaAction),DelayTime::create(0.2f), RotateTo::create(0, 0),RotateTo::create(0.5, rotaAction),Hide::create(),DelayTime::create(2.5),nullptr));
                    action->setTag(actionTag);
                    gravityPrompt->runAction(action);
                }
            }
            moveToTarget(lq,ls);
        }
    });
    addEndNum++;
    if(owner != nullptr){
        //        auto checkDistance = DistanceToTarget::create();
        //        checkDistance->setTargetNode(_operate->getNodeByName("limit"));
        //        checkDistance->addListners(MoveDistancePercent, [this](LQComponent*,OperateListner* lis){
        //            auto percent = dynAny_cast<float>(lis->getData());
        //            if(percent<0)
        //                percent = 0;
        //            else if(percent>1)
        //                percent = 1;
        //            auto bowl = _operate->getNodeByName("bowl");
        //            if(bowl != nullptr){
        //                this->setScale(1+0.4*percent);
        //                this->setPosition(moveDistance*percent);
        //            }
        //        });
        //        checkDistance->startUpdate();
        //        owner->addComponent(checkDistance);
        owner->addComponent(_con);
        _con->getFrameStatus()->setSearchAction(_operate->getAction());
    }
    return _con;
}

void BaseMakeLayer::layerScale(){
    vector<string> names = {"bowl","bowlUp"};
    for(auto name:names){
        auto bowl = _operate->getNodeByName(name);
        if(bowl != nullptr){
            auto frameState = FrameStatus::create(bowl->getName());
            frameState->setSearchAction(_operate->getAction());
            float time = 0.3;
            bowl->runAction(frameState->createAction(time,"pouring"));
        }
    }
}
void BaseMakeLayer::layerScaleBack(){
    vector<string> names = {"bowl","bowlUp"};
    for(auto name:names){
        auto bowl = _operate->getNodeByName(name);
        if(bowl != nullptr){
            auto frameState = FrameStatus::create(bowl->getName());
            frameState->setSearchAction(_operate->getAction());
            float time = 0.3;
            bowl->runAction(frameState->createAction(time,"start"));
        }
    }
}

void BaseMakeLayer::showBowl(){
    do {
        auto bowl = _operate->getNodeByName("bowl");
        CC_BREAK_IF(!bowl);
        auto noShade = bowl->getChildByName("noshade");
        if(noShade != nullptr){
            bowl->setCascadeOpacityEnabled(false);
            bowl->setOpacity(0);
        }
       // SoundPlayer::getInstance()->playEffect("sound/general/ingredients_fly_in.mp3");
        auto endfunc = [bowl,noShade](){
            if(noShade != nullptr){
                bowl->setOpacity(255);
            }
        };
        
        
        auto bowlUp = _operate->getNodeByName("bowlUp");
        if(bowlUp == nullptr) {
            ActionHelper::showBackInOut(bowl, bowl->getPosition(), ActionHelper::ShowDirection::show_from_left,endfunc);
            bowl->setRotation(200);
            bowl->runAction(EaseBackInOut::create(RotateTo::create(0.75, 360)));
            
        } else {
            bowlUp->getParent()->reorderChild(bowlUp, 3);
            ActionHelper::showBezier(bowl,bowl->getPosition(), ActionHelper::ShowDirection::show_from_left,endfunc);
            ActionHelper::showBezier(bowlUp,bowlUp->getPosition(), ActionHelper::ShowDirection::show_from_left);
            bowlUp->setVisible(true);
        }
        bowl->setVisible(true);
        
    }while (false);
    
}

void BaseMakeLayer::stopPrompt(){
    BaseStepLayer::stopPrompt();
    auto down = _operate->getNodeByName("down");
    if(down != nullptr){
        down->setVisible(false);
    }
}