//
//  HightUtil.cpp
//  IceCreamSundae
//
//  Created by luotianqiang on 16/5/28.
//
//

#include "HighLightUtil.h"
#define STRINGIFY(A)  #A
#include "HigthLightShader.frag"
USING_NS_CC;

void HighLightUtil::setHiLight(Node* node,float f){
    auto glProGramState = node->getGLProgramState();
    if(glProGramState == nullptr||glProGramState->getGLProgram() == nullptr||glProGramState->getGLProgram()->getUniform("highLight") == nullptr) {
        auto _glProgram  = GLProgramCache::getInstance()->getGLProgram( "highlightAction");
        if(_glProgram == nullptr) {
            _glProgram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, HigthLightShader_frag);
            CHECK_GL_ERROR_DEBUG();
            GLProgramCache::getInstance()->addGLProgram(_glProgram, "highlightAction");
        }
        glProGramState = GLProgramState::create(_glProgram);
        node->setGLProgramState(glProGramState);
        glProGramState = node->getGLProgramState();
    }
    glProGramState->setUniformFloat("highLight", f);
    CHECK_GL_ERROR_DEBUG();
}