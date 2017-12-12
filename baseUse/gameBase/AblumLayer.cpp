//
//  AblumLayer.cpp
//  MyDonutShop
//
//  Created by luotianqiang1 on 15/12/15.
//
//

#include "AblumLayer.h"
#include "FileUtility.h"
#include "MyDialog.h"
#include "RuntimePermission.h"
#include "RuntimePermissionManager.h"
string AblumLayer::mailcontent = "I just made a delicious Food at this SUPER FUNNN app!!! Download it for FREE now! See if you can make a better one!";
string AblumLayer::mailTitle = "";

IMPLEMENT_COCOS2DX_CLASS(AblumLayer);

AblumLayer* AblumLayer::create(const string& s){
    auto node = new AblumLayer();
    if(node) {
        node->filePath = s;
        node->init();
        node->autorelease();
        return node;
    }else {
        CC_SAFE_RELEASE_NULL(node);
        return nullptr;
    }
}

AblumLayer::AblumLayer() {
    closeCallback = nullptr;
    _image = nullptr;
    _shareFilePath = "";
    _maskPath = "";
    filePath = "popLayer/albumLayer.csb";
}

AblumLayer::~AblumLayer(){
    CC_SAFE_RELEASE_NULL(_image);
}

bool AblumLayer::init() {
    if(BaseStepLayer::init()) {
        _operate = StudioLayer::create(filePath);
        addChild(_operate);
        return true;
    }
    return false;
}

void AblumLayer::onEnterTransitionDidFinish(){
    BaseStepLayer::onEnterTransitionDidFinish();
    Director::getInstance()->getEventDispatcher()->setEnabled(false);
    ActionHelper::delayFunc(0.01, this, [this](){
        _operate->playNodeSound("showAudio");
        _operate->playAction("in",[this](){
            Director::getInstance()->getEventDispatcher()->setEnabled(true);
        });
        
    });
}

void AblumLayer::onExitTransitionDidStart(){
    BaseStepLayer::onExitTransitionDidStart();
#if __cplusplus > 201100L
    RuntimePermissionManager::getInstance()->onPermissionGrantedResult = nullptr;
#else
    RuntimePermissionManager::getInstance()->mRuntimePermissionDelegate = nullptr;
#endif
}

void AblumLayer::setRender(RenderTexture* _render){
    CC_SAFE_RELEASE_NULL(_image);
    _image = _render->newImage();
    
    Sprite* mask = nullptr;
    if(!_maskPath.empty())
        mask = Sprite::create(_maskPath);
    if(mask){
        auto popup_bg = _operate->getNodeByName("popup_bg");
        auto clipping = ClippingNode::create();
        auto _s = Sprite::createWithSpriteFrame(_render->getSprite()->getSpriteFrame());
        auto spSize = _s->getContentSize();
        auto _size = mask->getContentSize()+Size(10,10);
        float scaleX = _size.width/spSize.width;
        float scaleY = _size.height/spSize.height;
        auto _scale = max(scaleX, scaleY);
        _s->setScale(_scale);
        _s->setFlippedY(true);
        _s->setPosition(popup_bg->getContentSize()*.5);
        mask->setPosition(popup_bg->getContentSize()*.5);
        clipping->setAlphaThreshold(0.01f);
        clipping->setStencil(mask);
        clipping->addChild(_s);
        popup_bg->addChild(clipping,-1);
        
    }else {
        auto _contanier = _operate->getNodeByName("contanier");
        auto _s = Sprite::createWithSpriteFrame(_render->getSprite()->getSpriteFrame());
        auto spSize = _s->getContentSize();
        auto _size = _contanier->getContentSize();
        float scaleX = _size.width/spSize.width;
        float scaleY = _size.height/spSize.height;
        auto _scale = max(scaleX, scaleY);
        _s->setScale(_scale);
        _s->setFlippedY(true);
        _s->setPosition(_contanier->getContentSize()*.5);
        _contanier->addChild(_s);
    }
    
}

void AblumLayer::touchEnd(ui::Widget* widget){
    if(widget->getName() == "btn_ablum"){
        
#if __cplusplus > 201100L
        RuntimePermissionManager::getInstance()->onPermissionGrantedResult = [&](int           requestcode,bool bgranted){
            onPermissionGrantedResult(requestcode, bgranted);
        };
#else
        RuntimePermissionManager::getInstance()->mRuntimePermissionDelegate = this;
#endif
        
        //调用申请权限接口的标识，会在你的回调方法中用到，可以是任何值
        int requestCode = 1;
        //调用权限申请的方法,根据需要申请敏感权限
        RuntimePermissionManager::getInstance()->requestRuntimePermissions(requestCode, PERMISSION::kWriteExternalStorage);
        
        
        
    }else if(widget->getName() == "btn_share"){
        
#if __cplusplus > 201100L
        RuntimePermissionManager::getInstance()->onPermissionGrantedResult = [&](int           requestcode,bool bgranted){
            onPermissionGrantedResult(requestcode, bgranted);
        };
#else
        RuntimePermissionManager::getInstance()->mRuntimePermissionDelegate = this;
#endif
        
        //调用申请权限接口的标识，会在你的回调方法中用到，可以是任何值
        int requestCode = 2;
        //调用权限申请的方法,根据需要申请敏感权限
        RuntimePermissionManager::getInstance()->requestRuntimePermissions(requestCode, PERMISSION::kWriteExternalStorage);
        
        
    }else if(widget->getName() == "btn_close"){
        Director::getInstance()->getEventDispatcher()->setEnabled(false);
        _operate->playAction("out",[this](){
            this->retain();
            this->removeFromParent();
            if(closeCallback != nullptr)
                closeCallback();
            release();
            Director::getInstance()->getEventDispatcher()->setEnabled(true);
        });
    }
}

std::string AblumLayer::newSharePath(){
    string name = "shareCache";
    
    FileUtility::createDirectory((FileUtility::getStoragePath()+ "/"+name).c_str());
    FileUtility::createDirectory((FileUtility::getStoragePath()+ "/"+name +"/"+ "share").c_str());
    string rootPath;
    rootPath = FileUtility::getStoragePath()+"/"+name + "/" + "share" + "/";
    
    struct timeval tv;
    gettimeofday(&tv,NULL);
    long nowtime =  tv.tv_sec * 1000 + tv.tv_usec / 1000;
    
    auto sharePath = StringUtils::format("%s%ld.png",rootPath.c_str(),nowtime);
    return sharePath;
}

void AblumLayer::onPermissionGrantedResult(int requestCode,bool bGranted){
    
    if (bGranted) {
        this->runAction(Sequence::create(DelayTime::create(0.0f),
                                         CallFunc::create([=] {
            if(requestCode == 1){
                Director::getInstance()->getEventDispatcher()->setEnabled(false);
                _cfsys.saveToAlbum(_image, [this](bool _flag){
                    if(_flag) {
                        auto dialog = MyDialog::create("Image was saved to your Camera Roll!", MyDialog::oneBtnPath);
                        this->getScene()->addChild(dialog);
                    } else {
                        auto dialog = MyDialog::create("Failed, Please enable photo access in Privacy Setting!", MyDialog::oneBtnPath);
                        this->getScene()->addChild(dialog);
                    }
                    Director::getInstance()->getEventDispatcher()->setEnabled(true);
                },mailTitle);
            }else if(requestCode == 2){
                
                if(_image == nullptr) {
                    
                }
                if(_shareFilePath.empty()) {
                    _shareFilePath = newSharePath();
                    _image->saveToFile(_shareFilePath);
                }
                string content;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                string appId = "";
                auto map = FileUtils::getInstance()->getValueMapFromFile("AppConfig.plist");
                auto itor = map.find("AppleId");
                if(itor != map.end()){
                    auto value = itor->second;
                    if(value.getType() == Value::Type::STRING) {
                        appId = value.asString();
                    }
                }
                content = StringUtils::format("%s<p><a href=‘http://itunes.apple.com/app/%s’>http://itunes.apple.com/app/%s</a></p>",mailcontent.c_str(), appId.c_str(),appId.c_str());
#else
                content = mailcontent;
#endif
                _cfsys.sendEmailAndFilePic(mailTitle.c_str(), content.c_str(), _shareFilePath.c_str());
            }
        }),nullptr))    ;
        log("-------->anroid runtime permisson was granted,requestcode = %d",requestCode);
    }else{
        //add your code....
        log("-------->anroid runtime permisson was not  granted,requestcode = %d",requestCode);
    }
    
}