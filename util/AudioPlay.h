//
//  AudioPlay.h
//  candyfactory
//
//  Created by luotianqiang1 on 16/5/4.
//
//

#ifndef AudioPlay_hpp
#define AudioPlay_hpp

#include "cocos2d.h"
USING_NS_CC;
#include "AudioEngine.h"
using namespace std;
class AudioPlay {
public:
    static int play(const string& filePath);
};
#endif /* AudioPlay_hpp */
