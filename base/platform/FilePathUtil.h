//
//  FilePathUtil.hpp
//  makeburrito
//
//  Created by luotianqiang1 on 15/11/24.
//
//

#ifndef FilePathUtil_hpp
#define FilePathUtil_hpp
#include "FileUtility.h"
#include <iostream>
#include "cocos2d.h"
USING_NS_CC;
using namespace std;
class FilePathUtil {
public:
    static int getFileNum(string _parentPath);
    static void saveToFavorite(RenderTexture* render, function<void(bool)> function, string projDir);
    static void getFilesPathByDir(const string& path,vector<string>& v);
    static std::string newSharePath();
    static bool copyFile(const string& souceFile,const string& newFile);
protected:
    static unordered_map<string ,int> *numCache;
};

#endif /* FilePathUtil_hpp */
