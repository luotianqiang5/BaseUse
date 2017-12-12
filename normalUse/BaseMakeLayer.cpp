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
#include "BezierCreator.h"
BaseMakeLayer::BaseMakeLayer(){
    count =0;
    addEndNum = 0;
    spoonCheckPoint = Vec2(0.2,0.2);
    spoonRote=0;
    _mixAction = nullptr;
    mix = nullptr;
    isPouring = false;
    gravityPrompt = nullptr;
    grid = nullptr;
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
    if(grid)
            grid->stopAllActions();
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
        auto mixLayer = _operate->getNodeByName("mixLayer");
        if(mix == nullptr){
            mix = TimerChangeComponent::create();
            mix->setFrameFile(mixPath);
            mix->setDelayPerUnit(2.4f);
           mixLayer->addComponent(mix);
            mix->addListners(FrameChangeeEnd,CC_CALLBACK_2(BaseMakeLayer::mixEnd,this));
        }
        SoundPlayer::getInstance()->playEffect("sound/general/ingredients_fly_in.mp3");
   
        
        grid = NodeGrid::create();
        
        mixLayer->retain();
        mixLayer->getParent()->addChild(grid);
        mixLayer->removeFromParent();
        grid->addChild(mixLayer);
        grid->setPosition(mixLayer->getPosition());
        mixLayer->release();
        mixLayer->setPosition(Vec2::ZERO);
        
        moveSpoon = MoveHideShade::create();
        moveSpoon->setTarget(LQRect::create(CocosHelper::getNodeRectInWorld(_operate->getNodeByName("spoonLImit"))));
        moveSpoon->setCheckPoint(spoonCheckPoint);
        moveSpoon->setMoveType(MoveComponentType::kTypeBack);
        moveSpoon->addListners(ComponentTouchMove, [this,spoon](LQComponent*, OperateListner* _lis){
            if(moveSpoon->checkInlimit()){
                auto moveEndFunc = [=](){
               
                    moveSpoon->setEnabled(true);
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
                    auto bowlUp = _operate->getNodeByName("bowlUp");
                    if(bowlUp != nullptr)
                        bowlUp->getParent()->reorderChild(bowlUp, 501);
                    _lis->setEnable(false);
                    moveSpoon->setBrintTop(false);
                    spoon->setZOrder(0);
                    moveSpoon->setTarget(LQRect::create(CocosHelper::getNodeRectInWorld(_operate->getNodeByName("spoonLImit"))));
                    moveSpoon->setMoveType(MoveComponentType::kTypeInRect);
                    moveSpoon->addListners(ComponentTouchMove, [this](LQComponent*, OperateListner* _lis){
                        if(grid != nullptr){
                        auto actionTag = 2100;
                        if(!grid->getActionByTag(actionTag)) {
                            auto action = RepeatForever::create(Liquid::create(4, Size(4,4), 8,7));
                         
                            action->setTag(actionTag);
                            grid->runAction(action);
                           
                        }else
                            _actionManager->resumeTarget(grid);
                        }
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
                    auto end = [this](LQComponent*, OperateListner* _lis){
                        moveSpoon->stopSound();
                        mix->stop();
                        scaleBackBowl();
                        if(_mixAction != nullptr)
                            _actionManager->pauseTarget(mix->getOwner());
                        if(grid){
                             _actionManager->pauseTarget(grid);
                        }
                    };
                    moveSpoon->addListners(ComponentTouchEnd,end);
                    moveSpoon->addListners(ComponentTouchCancle, end);
                    moveSpoon->addListners(ComponentTouchMoveStand, end);
                };
                
                auto spoonFrame = FrameStatus::create(spoon->getName());
                spoonFrame->setSearchAction(_operate->getAction());
                auto spoonAction = spoonFrame->createAction(0.2f, "mixing");
                if(spoonAction != nullptr){
                     moveSpoon->setEnabled(false);
                       string nodeNames[] = {"bowl","bowlUp"};
                    for(int i=0;i<sizeof(nodeNames)/sizeof(*nodeNames);i++){
                        auto scaleNode = _operate->getNodeByName(nodeNames[i]);
                        if(scaleNode != nullptr){
                            auto frame = FrameStatus::create(scaleNode->getName());
                            frame->setSearchAction(_operate->getAction());
                            auto action = frame->createAction(0.8, "mixing");
                            if(action != nullptr)
                                scaleNode->runAction(action);
                        }
                    }
                    spoon->runAction(Sequence::create(spoonAction, CallFunc::create(moveEndFunc),nullptr));
                }else {
                    moveEndFunc();
                }
            }
        });
        spoon->addComponent(moveSpoon);
        moveSpoon->setEnabled(false);
        ActionHelper::showBackInOut(spoon, spoon->getPosition(), ActionHelper::ShowDirection::show_from_right,[=](){
            auto mixPrompt = _operate->getNodeByName("mixPrompt");
            if(mixPrompt != nullptr){
                mixPrompt->stopAllActions();
                mixPrompt->setVisible(true);
                mixPrompt->runAction(RepeatForever::create(RotateBy::create(1, 90)));
            }
             moveSpoon->setEnabled(true);
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
            ActionHelper::showBackInOut(bowl, bowl->getPosition(), ActionHelper::ShowDirection::show_from_left,endfunc,1.1);
            bowl->setRotation(200);
            bowl->runAction(EaseBackInOut::create(RotateTo::create(1.1, 360)));
            
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

void BaseMakeLayer::showAddPrompt(){
    if(_prompt != nullptr){
        vector<Vec2> points = {
            _prompt->getPosition(),
            _prompt->getPosition()+Vec2(-140,130),
            _prompt->getPosition()+Vec2(-330,90),
            _operate->getNodeByName("bowl")->getPosition()
        };
        _prompt->setVisible(true);
          auto down = _operate->getNodeByName("down");
        auto se = Sequence::create(CallFunc::create([=](){
            if(down != nullptr)
                down->setVisible(true);
        }), FadeTo::create(0, 255), BezierCreator::createAction(points, 1),CallFunc::create([=](){
            if(down != nullptr)
                down->setVisible(false);
        }),FadeTo::create(0, 0), DelayTime::create(2.7f),nullptr);
        _prompt->runAction(RepeatForever::create(se));
        if(down != nullptr)
            down->setVisible(true);
    }
}

void BaseMakeLayer::hideThing(Node* node,float delayTime){
    ActionHelper::delayFunc(delayTime, this, [this,node]{
        Director::getInstance()->getEventDispatcher()->setEnabled(true);
        ActionHelper::ShowDirection dir = ActionHelper::ShowDirection::show_from_right;
        auto bowl = _operate->getNodeByName("bowl");
        if(bowl != nullptr) {
            if(node->getPositionX()<bowl->getPositionX())
                dir = ActionHelper::ShowDirection::show_from_left;
        }
        node->runAction(RotateTo::create(0.6, 0));
        ActionHelper::hide(node, dir);
        auto thingBowl  = _operate->getNodeByName(node->getName() + "bowl");
        if(thingBowl) {
            ActionHelper::hide(thingBowl, dir);
        }
    });
}

void BaseMakeLayer::changeNodeToClipping(const vector<std::string>& nodeNames){
    for(auto name:nodeNames) {
        auto node = _operate->getNodeByName(name);
        if(node != nullptr){
            auto s = dynamic_cast<Sprite*>(node);
            if(s != nullptr) {
                s->setCascadeOpacityEnabled(false);
                s->setName("");
                s->setOpacity(0);
                auto clip = ClippingNode::create();
                s->addChild(clip);
                clip->setAlphaThreshold(0.5f);
                auto mask = Sprite::createWithSpriteFrame(s->getSpriteFrame());
                mask->setPosition(s->getContentSize()*.5);
                clip->setStencil(mask);
                
                auto inner = Sprite::createWithSpriteFrame(s->getSpriteFrame());
                inner->setPosition(s->getContentSize()*.5);
                clip->addChild(inner);
                inner->setName("inner");
            }
        }
    }
}