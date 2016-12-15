//
//  ColoringLineFrameNode.cpp
//  ScribbleDemo_3_2
//
//  Created by renhong on 8/17/15.
//
//

#include "ColoringLineFrameNode.h"
#include "ScribbleUtil.h"

#pragma mark - ColoringLineFrameNode

ColoringLineFrameNode *ColoringLineFrameNode::createWithCanvas(RenderTexture *pCanvas){
    ColoringLineFrameNode *ret = new ColoringLineFrameNode();
    if(ret && ret->init(pCanvas)){
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ColoringLineFrameNode::init(RenderTexture *pCanvas){
    if (!Node::init()) {
        return false;
    }
    this->setContentSize(pCanvas->getSprite()->getContentSize());
    this->_canvas = pCanvas;
    this->_canvas->retain();
    this->initRenderPara();
    return true;
}

ColoringLineFrameNode::ColoringLineFrameNode(){
    _patternNode = nullptr;
    _delegate = nullptr;
    _debugPattern = false;
}

ColoringLineFrameNode::~ColoringLineFrameNode(){
    
    this->setPatternNode(nullptr);
}

bool ColoringLineFrameNode::init(string pLineArtFile){
    if (!ColoringNode::init(pLineArtFile, pLineArtFile)) {
        return false;
    }
    _debugPattern = false;
    return true;
}

void ColoringLineFrameNode::initPattern(string pPatternImage){
    //overwrite父类方法，以此避免在init方法中被调用执行ColoringNode::initPattern
}

void ColoringLineFrameNode::updatePattern(){
    GLProgramState *lState = GLProgramState::getOrCreateWithGLProgram(this->getTemplate()->getGLProgram());
    lState->setUniformTexture("s_texture_shape", this->getTemplate()->getTexture());
    lState->setUniformVec2("v_texture_shape_size", this->getTemplate()->getTexture()->getContentSize());
    CC_SAFE_RELEASE(_templateImage);
    _templateImage = this->getPatternNode()->getPatternImage();
    _templateImage->retain();
    CC_SAFE_RELEASE(_shapeBoundTexture);
    _shapeBoundTexture = this->getShapeBounds(_templateImage);
    _shapeBoundTexture->retain();
    lState->setUniformTexture("s_texture_shape_bounds", _shapeBoundTexture);
}

bool ColoringLineFrameNode::setPattern(string pLineFrameFile,int pTolerance/* = 0*/){
    if (this->getPatternNode()) {
        this->getPatternNode()->removeFromParent();
    }

    Pattern *lPatternNode = Pattern::create(pLineFrameFile);
    lPatternNode->setDelegate(this);
    lPatternNode->setBoundaryTolerance(pTolerance);
    this->setPatternNode(lPatternNode);
    this->addChild(lPatternNode, -1);
//    lPatternNode->setAnchorPoint(Vec2(0.5, 0.5));
    lPatternNode->setVisible(false);
    this->getTemplate()->setTexture(this->getPatternNode()->getLineArt()->getTexture());
    this->getTemplate()->setContentSize(this->getPatternNode()->getLineArt()->getContentSize());
    lPatternNode->fillPatternAsync();
    
    return true;
}

void ColoringLineFrameNode::debugPattern(bool pDebug/* = true*/){
    if (!pDebug) {
        return;
    }
    _debugPattern = true;
    if(nullptr != _lineArt){
        _lineArt->setVisible(false);
    }
    if(nullptr != this->getPatternNode()){
        this->getPatternNode()->setVisible(true);
    }
    if(nullptr != _canvas){
        _canvas->clear(0.0, 0.0, 0.0, 0.0);
    }
}

void ColoringLineFrameNode::loadPercent(Pattern *pNode, float pPercent){
    this->getDelegate()->loadPercent(this, pPercent);
}

void ColoringLineFrameNode::loadEnded(Pattern *pNode){
    this->updatePattern();
    this->getDelegate()->loadEnded(this);
    if (_debugPattern) {
        this->debugPattern(true);
        string lPath = FileUtils::getInstance()->getWritablePath() + "pattern_debug.png";
        this->getPatternNode()->savePattern(lPath);
    }
}

#pragma mark - ColoringLineFrameTouchNode

ColoringLineFrameTouchNode *ColoringLineFrameTouchNode::create(string pLineArtFile){
    ColoringLineFrameTouchNode *ret = new ColoringLineFrameTouchNode();
    if(ret && ret->init(pLineArtFile)){
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ColoringLineFrameTouchNode::init(string pLineArtFile){
    if (!ColoringLineFrameNode::init(pLineArtFile)) {
        return false;
    }
    //响应点击事件：根据点击位置，够造相应着色参数给着色器。
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](cocos2d::Touch *pTouch, cocos2d::Event *pEvent)->bool{
        //对于无边框的线框图图片，是否认为图片边界为边框。
        if (this->getIgnoreOutOfLineframe()) {//如果忽略线框图外部区域，则判断点击点是否在线框之内。
            if (this->insideLineframe(pTouch->getLocation())) {
            }
            else{
                return false;//如果点击点不在线框之内，则不填色。
            }
        }
        
        return this->coloring(pTouch->getLocation());
    };
    listener->onTouchMoved = [](cocos2d::Touch *pTouch, cocos2d::Event *pEvent)->void{};
    listener->onTouchEnded = [](cocos2d::Touch *pTouch, cocos2d::Event *pEvent)->void{};
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}