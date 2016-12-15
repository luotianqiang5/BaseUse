//
//  CSVParse.h
//  CodeBase
//
//  Reconstruction by zhangguangzong on 15-12-29.
//
//

#ifndef CODEBASE_CORE_UTILS_CSVPARSE_H_
#define CODEBASE_CORE_UTILS_CSVPARSE_H_


#include <iostream>
#include <stdio.h>
#include <fstream>
#include "cocos2d.h"

using namespace std;
USING_NS_CC;

class CSVParse : public Ref
{
public:
    /** creator **/
    static CSVParse* create(const char* csvFileName);
    
    /** get data by rows & columns **/
    const char* getData(unsigned int rows,unsigned int cols);
    
	const char* getDatas(unsigned int rows,const char* colsName);
    
    int findColsData(unsigned int rows,const char* value);
    
    inline const vector<vector<string>>& getAllDatas() const
    {
        return _datas;
    };
    
    inline  long getCols() const
    { return _colNum; }
    
    inline  long getRows() const
    { return _datas.size(); }
    
    
protected:
    /** constructor **/
    CSVParse();
    
    /** distructor **/
    ~ CSVParse();
    
    /** load csv file **/
    bool loadFile(const char* fileName);
protected:
    long _colNum;
    /** separator characters **/
    string _fieldSeparator;
    
    /** data container **/
    vector<vector<string>> _datas;
private:
    /** split strings **/
    void split(vector<string>* field,string line);
    
    /** return next index of separator &  spited string **/
    long advplain(const string& line, string* splitStr, long index);
    
    /** split quoted string  return next index of separator **/
    long advquoted(const string& line, string* splitStr, long index);
    
};

#endif /* defined(CODEBASE_CORE_UTILS_CSVPARSE_H_) */
