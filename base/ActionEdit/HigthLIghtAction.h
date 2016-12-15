//
//  LoadingBarAction.hpp
//  candyfactory
//
//  Created by luotianqiang1 on 16/4/11.
//
//

#ifndef HigthLIghtAction_hpp
#define HigthLIghtAction_hpp
#include "cocos2d.h"
#include "HighLightUtil.h"
USING_NS_CC;

/**
 @brief Progress from a percentage to another percentage.
 @since v0.99.1
 */
class  HigthLIghtAction : public ActionInterval
{
public:
    
    static HigthLIghtAction* create(float duration, float fromPercentage, float toPercentage);
    
    //
    // Overrides
    //
    virtual HigthLIghtAction* clone() const override;
    virtual HigthLIghtAction* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    virtual void update(float time) override;
    

    HigthLIghtAction() {}
    virtual ~HigthLIghtAction() {}
    
    bool initWithDuration(float duration, float fromPercentage, float toPercentage);
protected:
    float _to;
    float _from;
private:
    CC_DISALLOW_COPY_AND_ASSIGN(HigthLIghtAction);
};

// end of actions group
/// @}


#endif /* HigthLIghtAction_hpp */
