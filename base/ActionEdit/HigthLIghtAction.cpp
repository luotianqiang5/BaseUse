//
//  LoadingBarAction.cpp
//  candyfactory
//
//  Created by luotianqiang1 on 16/4/11.
//
//

#include "HigthLIghtAction.h"


// implementation of HigthLIghtAction

HigthLIghtAction* HigthLIghtAction::create(float duration, float fromPercentage, float toPercentage)
{
    auto higthLIghtAction = new HigthLIghtAction();
    higthLIghtAction->initWithDuration(duration, fromPercentage, toPercentage);
    higthLIghtAction->autorelease();
    
    return higthLIghtAction;
}

bool HigthLIghtAction::initWithDuration(float duration, float fromPercentage, float toPercentage)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _to = toPercentage;
        _from = fromPercentage;
        
        return true;
    }
    
    return false;
}

HigthLIghtAction* HigthLIghtAction::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) HigthLIghtAction();
    a->initWithDuration(_duration, _from, _to);
    a->autorelease();
    return a;
}


HigthLIghtAction* HigthLIghtAction::reverse() const
{
    return HigthLIghtAction::create(_duration, _to, _from);
}

void HigthLIghtAction::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
}

void HigthLIghtAction::update(float time)
{
    float data = _from + (_to - _from) * time;
    HighLightUtil::setHiLight(_target, data);
}

