//
//  MultiSprite.cpp
//  Milkshake
//
//  Created by luotianqiang1 on 4/13/15.
//
//

#include "MultiSprite.h"
#include <math.h>
USING_NS_KD;
MultiSprite::MultiSprite() {
    moveRect.size= Director::getInstance()->getWinSize();
    moveRect.origin = Vec2::ZERO;
    touchDirection = Vec2::ZERO;
    touchPoint = Vec2::ZERO;
    maxScale = 1;
    minScale = 0.5;
    length = 0;
    _limit = nullptr;
    _polygon = nullptr;
    _eventType = TYPENULL;
}
MultiSprite::~MultiSprite() {
    CC_SAFE_RELEASE_NULL(_limit);
    CC_SAFE_RELEASE_NULL(_polygon);
}

bool MultiSprite::initWithFile(const std::string& filename) {
    auto flag = EventSprite::initWithFile(filename);
    focusRect = Rect(0,0,getContentSize().width,getContentSize().height);
    _polygon = LQRect::create(Rect(0,0,getContentSize().width,getContentSize().height));
    _polygon->retain();
    return flag;
}

void MultiSprite::setFocusrect(Rect rect) {
    this->focusRect = rect;
}

void MultiSprite::setMoveRect(Rect rect) {
    this->moveRect = rect;
}

void MultiSprite::setCenterPoint(Vec2 point) {
    centerPoint.clear();
    centerPoint.push_back(point);
}

MultiSprite* MultiSprite::create(const std::string& filename) {
    auto *sprite = new (std::nothrow) MultiSprite();
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

MultiSprite* MultiSprite::createWithTex(Texture2D* tex) {
    auto *sprite = new (std::nothrow) MultiSprite();
    if (sprite && sprite->initWithTexture(tex))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

MultiSprite* MultiSprite::createWithTex(Texture2D* tex,Rect r){
    auto *sprite = new (std::nothrow) MultiSprite();
    if (sprite && sprite->initWithTexture(tex, r))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool MultiSprite::onTouchBegan(Touch*pTouch, Event *pEvent) {
    auto pos = convertToNodeSpace(pTouch->getLocation());
    if(!focusRect.containsPoint(pos)) {
        if(_touches.size()==0)
            return false;
    }
    if(_touches.size()>=2)
        return true;
    
    if(std::find(_touches.begin(), _touches.end(), pTouch) == _touches.end())
        _touches.push_back(pTouch);
    if(_touches.size() ==1) {
        if(ornPosition.size() == 0) {
            ornPosition.push_back(getParent()->convertToWorldSpace(getPosition()));
            ornPosition.push_back(getAnchorPoint());
            ornScale = getScale();
        }
        touchPoint = pTouch->getLocation();
        touchCache.clear();
        touchCache.push_back(touchPoint);
        
        _eventType = TYPENULL;
        points.clear();
    }
    else if(_touches.size() == 2) {
        touchPoint = _touches[0]->getLocation().getMidpoint(_touches[1]->getLocation());
        length = _touches[0]->getLocation().distance(_touches[1]->getLocation());
        touchDirection = _touches[0]->getLocation()-_touches[1]->getLocation();
    }
    return true;
}
void MultiSprite::onTouchMoved(Touch*pTouch, Event *pEvent) {
    if(std::find(_touches.begin(), _touches.end(), pTouch) != _touches.end()) {
        if(_touches.size() == 1) {
            auto newPoint = pTouch->getLocation();
            auto disance = newPoint-touchPoint;
            touchPoint = newPoint;
            
            if(_eventType == TYPENULL) {
                if(points.size() == 0){
                    auto dis = convertToNodeSpace(pTouch->getLocation()).distance(getContentSize()*.5);
                    auto per = dis/Vec2(getContentSize()*.5).getLength();
                    
                    if(per<0.2)
                        _eventType = TYPEMOVE;
                    else
                        _eventType = TYPENULL;
                }
                
                if(points.size()<6){
                    points.push_back(pTouch->getLocation());
                }
                else {
                    float sum = 0;
                    for(int i=0;i<points.size()-2;i++){
                        auto a1 = points[i+1]-points[i];
                        auto a2 = points[i+2]-points[i+1];
                        sum+=a2.getAngle(a1);
                    }
                    if(std::abs(sum)<0.25)
                        _eventType = TYPEMOVE;
                    else
                        _eventType = TYPEROTE;
                }
            } else if(_eventType == TYPEROTE) {
                points.clear();
                auto _ownPos = convertToWorldSpace(getContentSize()*.5);
                Vec2 temp1 = pTouch->getPreviousLocation() - _ownPos;
                Vec2 temp2 = pTouch->getLocation() - _ownPos;
                float theAngle = CC_RADIANS_TO_DEGREES(temp1.getAngle(temp2));
                rote(-theAngle);
            }else{
                
                if(points.size()>0)
                    disance = newPoint-points.at(0);
                points.clear();
                move(disance);
            }
        }
        else if(_touches.size() == 2) {
            
            auto newPoint = _touches[0]->getLocation().getMidpoint(_touches[1]->getLocation());
            auto disance = newPoint-touchPoint;
            touchPoint = newPoint;
            
            auto curentLen =  _touches[0]->getLocation().distance(_touches[1]->getLocation());
            auto distanceLen = curentLen-length;
            length = curentLen;
            
            
            auto curentDirection = _touches[0]->getLocation()-_touches[1]->getLocation();
            auto degree = Vec2::angle(curentDirection, touchDirection);
            Vec3 v1 = Vec3(curentDirection.x,curentDirection.y,0);
            Vec3 v2 = Vec3(touchDirection.x,touchDirection.y,0);
            v1.cross(v2);
            if(v1.z<0)
                degree=-degree;
            touchDirection = curentDirection;
            
            auto ro = degree/3.1415926*180;
            CCLOG("ro==%f,dis==%f",degree*distanceLen,distanceLen);
            rote(ro);
            scale(distanceLen);
        }
    }
}

void  MultiSprite::rote(float distacedegree) {
    auto touchPoint = this->touchPoint;
    if(centerPoint.size()>0)
        touchPoint = centerPoint.at(0);
    bool isContain = true;
    if(_limit != nullptr) {
        auto center = convertToWorldSpace(getContentSize()*.5);
        for(const auto & t:_polygon->getPolygonPoits()){
            auto v = convertToWorldSpace(t);
            v.rotate(center, distacedegree);
            if(!PolygonHelp::isPointInPolygon(v,_limit->getPolygonPoits())){
                isContain = false;
                break;
            }
        }
    }
    
    if(isContain)
        setRotation(getRotation()+distacedegree);
}

void MultiSprite::scale(float distace) {
    auto touchPoint = this->touchPoint;
    if(centerPoint.size()>0)
        touchPoint = centerPoint.at(0);
    
    auto scale = distace/Vec2(getContentSize()).getLength();
    auto newscale = scale+getScale();
    
    
    if(newscale>maxScale||newscale<minScale)
        return;
    auto orns = getScale();
    setScale(newscale);
    if(_limit != nullptr) {
        for(const auto & t:_polygon->getPolygonPoits()){
            auto v = convertToWorldSpace(t);
            if(!PolygonHelp::isPointInPolygon(v,_limit->getPolygonPoits())){
                setScale(orns);
                break;
            }
        }
    }
}

void MultiSprite::resetSprite() {
    if(ornPosition.size()>1) {
        setPosition(getParent()->convertToNodeSpace(ornPosition.at(0)));
        setAnchorPoint(ornPosition.at(1));
        setScale(ornScale);
        setRotation(0);
    }
    
}

void MultiSprite::move(Vec2 distance) {
    auto p = convertToWorldSpace(getContentSize()*.5)+distance;
    bool isContain = true;
    if(_limit != nullptr) {
        for(const auto & t:_polygon->getPolygonPoits()){
            auto v = convertToWorldSpace(t)+distance;
            if(!PolygonHelp::isPointInPolygon(v,_limit->getPolygonPoits())){
                isContain = false;
                break;
            }
        }
    }
    //    if(distance.y>0 && p.y>moveRect.getMaxY())
    //            return;
    //    else if(distance.y<0&& p.y<moveRect.getMinY())
    //            return;
    //    if(distance.x>0&&p.x>moveRect.getMaxX())
    //            return;
    //    else if(distance.x<0&&p.x<moveRect.getMinX())
    //            return;
    if(isContain)
        setPosition(getPosition()+distance);
}

void MultiSprite::onTouchEnded(Touch* pTouch, Event *pEvent) {
    auto it = std::find(_touches.begin(), _touches.end(), pTouch);
    if(it != _touches.end()) {
        _touches.erase(it);
        if(_touches.size() == 1)
            touchPoint = _touches[0]->getLocation();
        else if(_touches.size() == 0)
            touchCache.clear();
    }
    
    
}
void MultiSprite::onTouchCancle(Touch* pTouch, Event *pEvent) {
    auto it = std::find(_touches.begin(), _touches.end(), pTouch);
    if(it != _touches.end()) {
        _touches.erase(it);
        if(_touches.size() == 1)
            touchPoint = _touches[0]->getLocation();
        else if(_touches.size() == 0)
            touchCache.clear();
    }
    
}

