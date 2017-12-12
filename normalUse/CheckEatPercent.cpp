//
//  CheckEatPercent.cpp
//  italianchef
//
//  Created by luotianqiang1 on 15/10/29.
//
//

#include "CheckEatPercent.h"
CheckEatPercent::CheckEatPercent(){
    _scheduler = Director::getInstance()->getScheduler();
     _pixcelBegin = 0;
     _pixcelEnd = 0;
    _image = nullptr;
    eatPercentageCallback = nullptr;
    renderSize= Director::getInstance()->getVisibleSize();
    _enabled = true;
    checkPercent = 2;
}

bool CheckEatPercent::init(){
    return true;
}
void CheckEatPercent::setEnabled(bool b){
    Component::setEnabled(b);
    if(b && !_scheduler->isScheduled(schedule_selector(CheckEatPercent::checkSchedle),this))
       _scheduler->schedule(schedule_selector(CheckEatPercent::checkSchedle), this, checkPercent , CC_REPEAT_FOREVER, 0.0f, false);
    else if(!b&&_scheduler->isScheduled(schedule_selector(CheckEatPercent::checkSchedle),this))
         _scheduler->unschedule(schedule_selector(CheckEatPercent::checkSchedle), this);
}

void CheckEatPercent::reset(){
    _pixcelBegin = 0;
    _pixcelEnd = 0;
}
void CheckEatPercent::addCheckNode(Node* _node){
    if(_node != nullptr)
        _checkNodes.pushBack(_node);
}

void CheckEatPercent::deletNode(int index){
    if(index>=0&&index<_checkNodes.size())
        _checkNodes.erase(index);
}

void CheckEatPercent::onEnter(){
    Component::onEnter();
    if(_enabled && !_scheduler->isScheduled(schedule_selector(CheckEatPercent::checkSchedle),this))
        _scheduler->schedule(schedule_selector(CheckEatPercent::checkSchedle), this, checkPercent , CC_REPEAT_FOREVER, 0.0f, false);
    
}
void CheckEatPercent::onExit(){
    Component::onExit();
    _scheduler->unschedule(schedule_selector(CheckEatPercent::checkSchedle), this);
}

//获取不透明像素点
size_t CheckEatPercent::calcute(Image *pImage)
{
    size_t lCounter = 1;
    for (int i = 0; i < pImage->getHeight(); ++i) {
        for (int j = 0; j < pImage->getWidth(); ++j) {
            unsigned char *lData = pImage->getData();
            size_t lPixcelIndex = i * pImage->getWidth() + j;
            unsigned char lRed = lData[lPixcelIndex * 4];
            unsigned char lGreen = lData[lPixcelIndex * 4 + 1];
            unsigned char lBlue = lData[lPixcelIndex * 4 + 2];
            unsigned char lAlpha = lData[lPixcelIndex * 4 + 3];
            if (lAlpha > 10) {
                if (lRed > 0 || lGreen > 0 || lBlue > 0) {
                    ++lCounter;
                }
            }
        }
    }
    return lCounter;
}

 void CheckEatPercent::checkSchedle(float){
     if(!_enabled||_checkNodes.size()==0)
         return ;
 
     RenderTexture* render = RenderTexture::create(renderSize.width, renderSize.height, Texture2D::PixelFormat::RGBA8888, GL_DEPTH24_STENCIL8);
     render->begin();
     for(auto _node:_checkNodes)
         _node->visit();
     render->end();
        Director::getInstance()->getRenderer()->render();
     _image = render->newImage();
     if(_pixcelBegin == 0)
     {
         render->begin();
         for(auto _node:_checkNodes)
             _node->visit();
         render->end();
         _pixcelBegin = calcute(_image);
         if(_pixcelBegin==0)
             _pixcelBegin = 1;
         delete _image;
         return;
     }
     _pixcelEnd = calcute(_image);
     delete _image;
     if(nullptr != eatPercentageCallback)
         eatPercentageCallback(1 - _pixcelEnd/(_pixcelBegin *1.0));
}