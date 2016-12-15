//
//  AblumLayer.hpp
//  MyDonutShop
//
//  Created by luotianqiang1 on 15/12/15.
//
//

#ifndef AblumLayer_hpp
#define AblumLayer_hpp

#include "BaseStepLayer.h"
class AblumLayer: public BaseStepLayer{
    DECLARE_DYNCLASS(AblumLayer);
    CC_SYNTHESIZE(string, _maskPath, MaskPath)
public:
    static AblumLayer* create(const string&);
    CREATE_FUNC(AblumLayer);
    AblumLayer();
    ~AblumLayer();
    void setRender(RenderTexture* _render);
protected:
    virtual bool init();
    virtual void onEnterTransitionDidFinish();
    virtual void touchEnd(ui::Widget* widget);
    std::string newSharePath();
protected:
    Image* _image;
    string _shareFilePath;
    string filePath;
public:
    std::function<void()> closeCallback;
    static string mailcontent;
    static string mailTitle;
};

#endif /* AblumLayer_hpp */
