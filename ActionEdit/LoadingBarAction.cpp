//
//  LoadingBarAction.cpp
//  candyfactory
//
//  Created by luotianqiang1 on 16/4/11.
//
//

#include "LoadingBarAction.h"
#include "ui/CocosGUI.h"
using namespace ui;

#define kProgressTimerCast  ui::LoadingBar*

LoadingTo* LoadingTo::create(float duration, float percent)
{
   
    auto loadingTo = new  LoadingTo();
    loadingTo->initWithDuration(duration, percent);
    loadingTo->autorelease();
    
    return loadingTo;
}

bool LoadingTo::initWithDuration(float duration, float percent)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _to = percent;
        
        return true;
    }
    
    return false;
}

LoadingTo* LoadingTo::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) LoadingTo();
    a->initWithDuration(_duration, _to);
    a->autorelease();
    return a;
}

LoadingTo* LoadingTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in LoadingTo");
    return nullptr;
}

void LoadingTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _from = ((kProgressTimerCast)(target))->getPercent();
}

void LoadingTo::update(float time)
{
    ((kProgressTimerCast)(_target))->setPercent(_from + (_to - _from) * time);
}

// implementation of LoadingFromTo

LoadingFromTo* LoadingFromTo::create(float duration, float fromPercentage, float toPercentage)
{
    auto loadingFromTo = new LoadingFromTo();
    loadingFromTo->initWithDuration(duration, fromPercentage, toPercentage);
    loadingFromTo->autorelease();
    
    return loadingFromTo;
}

bool LoadingFromTo::initWithDuration(float duration, float fromPercentage, float toPercentage)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _to = toPercentage;
        _from = fromPercentage;
        
        return true;
    }
    
    return false;
}

LoadingFromTo* LoadingFromTo::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) LoadingFromTo();
    a->initWithDuration(_duration, _from, _to);
    a->autorelease();
    return a;
}


LoadingFromTo* LoadingFromTo::reverse() const
{
    return LoadingFromTo::create(_duration, _to, _from);
}

void LoadingFromTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
}

void LoadingFromTo::update(float time)
{
    ((kProgressTimerCast)(_target))->setPercent(_from + (_to - _from) * time);
}