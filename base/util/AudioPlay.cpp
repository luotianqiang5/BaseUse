//
//  AudioPlay.cpp
//  candyfactory
//
//  Created by luotianqiang1 on 16/5/4.
//
//

#include "AudioPlay.h"
using namespace cocos2d::experimental;

int AudioPlay::play(const string& filePath){
    auto path = FileUtils::getInstance()->fullPathForFilename(filePath);
    int id = AudioEngine::play2d(path);
    return id;
}