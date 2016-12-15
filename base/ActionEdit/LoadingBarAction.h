//
//  LoadingBarAction.hpp
//  candyfactory
//
//  Created by luotianqiang1 on 16/4/11.
//
//

#ifndef LoadingBarAction_hpp
#define LoadingBarAction_hpp
#include "cocos2d.h"
USING_NS_CC;


class  LoadingTo : public ActionInterval
{
public:
    /**
     * @brief Create and initializes with a duration and a destination percentage.
     * @param duration Specify the duration of the LoadingTo action. It's a value in seconds.
     * @param percent Specify the destination percentage.
     * @return If the creation sucess, return a pointer of LoadingTo action; otherwise, return nil.
     */
    static LoadingTo* create(float duration, float percent);
    
    //
    // Overrides
    //
    virtual LoadingTo* clone() const override;
    virtual LoadingTo* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    virtual void update(float time) override;
    

    LoadingTo() {}
    virtual ~LoadingTo() {}
    
    /**
     * @brief Initializes with a duration and destination percentage.
     * @param duration Specify the duration of the LoadingTo action. It's a value in seconds.
     * @param percent Specify the destination percentage.
     * @return If the creation sucess, return true; otherwise, return false.
     */
    bool initWithDuration(float duration, float percent);
    
protected:
    float _to;
    float _from;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(LoadingTo);
};

/**
 @brief Progress from a percentage to another percentage.
 @since v0.99.1
 */
class  LoadingFromTo : public ActionInterval
{
public:
    /**
     * @brief Create and initializes the action with a duration, a "from" percentage and a "to" percentage.
     * @param duration Specify the duration of the LoadingFromTo action. It's a value in seconds.
     * @param fromPercentage Specify the source percentage.
     * @param toPercentage Specify the destination percentage.
     * @return If the creation sucess, return a pointer of LoadingFromTo action; otherwise, return nil.
     */
    static LoadingFromTo* create(float duration, float fromPercentage, float toPercentage);
    
    //
    // Overrides
    //
    virtual LoadingFromTo* clone() const override;
    virtual LoadingFromTo* reverse() const override;
    virtual void startWithTarget(Node *target) override;
    virtual void update(float time) override;
    

    LoadingFromTo() {}
    virtual ~LoadingFromTo() {}
    
    /**
     * @brief Initializes the action with a duration, a "from" percentage and a "to" percentage.
     * @param duration Specify the duration of the LoadingFromTo action. It's a value in seconds.
     * @param fromPercentage Specify the source percentage.
     * @param toPercentage Specify the destination percentage.
     * @return If the creation sucess, return true; otherwise, return false.
     */
    bool initWithDuration(float duration, float fromPercentage, float toPercentage);
    
protected:
    float _to;
    float _from;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(LoadingFromTo);
};

// end of actions group
/// @}


#endif /* LoadingBarAction_hpp */
