#include "codebase/core/utils/FrameWorkHelper.h"

void FrameWorkHelper::split(const std::string& src, const std::string& token, strArray* strs)
{
    size_t nend = 0;
    size_t nbegin = 0;
    size_t tokenSize = token.size();
    while(nend != std::string::npos)
    {
        nend = src.find(token, nbegin);
        if(nend == std::string::npos)
            strs->push_back(src.substr(nbegin, src.length()-nbegin));
        else
            strs->push_back(src.substr(nbegin, nend-nbegin));
        nbegin = nend + tokenSize;
    }
}

bool FrameWorkHelper::splitWithForm(const std::string& content, strArray* strs)
{
    bool bRet = false;
    
    do
    {
        CC_BREAK_IF(content.empty());
        
        size_t nPosLeft  = content.find('{');
        size_t nPosRight = content.find('}');
        
        // don't have '{' and '}'
        CC_BREAK_IF(nPosLeft == std::string::npos || nPosRight == std::string::npos);
        // '}' is before '{'
        CC_BREAK_IF(nPosLeft > nPosRight);
        
        const std::string pointStr = content.substr(nPosLeft + 1, nPosRight - nPosLeft - 1);
        // nothing between '{' and '}'
        CC_BREAK_IF(pointStr.length() == 0);
        
        size_t nPos1 = pointStr.find('{');
        size_t nPos2 = pointStr.find('}');
        // contain '{' or '}'
        CC_BREAK_IF(nPos1 != std::string::npos || nPos2 != std::string::npos);
        
        split(pointStr, ",", strs);
        
        bRet = true;
    } while (0);
    
    return bRet;
}