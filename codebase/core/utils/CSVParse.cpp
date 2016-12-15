#include "codebase/core/utils/CSVParse.h"


CSVParse::CSVParse():_fieldSeparator(","),_colNum(0)
{
    
}

CSVParse::~CSVParse()
{
    // release datas, free memory
    for(auto itr = _datas.begin(); itr != _datas.end(); ++itr)
    {
        (*itr).clear();
    }
    _datas.clear();
    
}

CSVParse* CSVParse::create(const char *csvFileName)
{
    CSVParse* pRet = new CSVParse;
    if (pRet && pRet->loadFile(csvFileName))
    {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool CSVParse::loadFile(const char *fileName)
{
    string pathKey = FileUtils::getInstance()->fullPathForFilename(fileName);
	const char* xmlConfFullPath = pathKey.c_str();
    ssize_t size = 0;
    
	//if ( access(xmlConfFullPath , 0 ) != 0 ) return false;
    
	char *pBuffer = (char*)FileUtils::getInstance()->getFileData(xmlConfFullPath , "rb" , &size );
	if ( pBuffer )
	{
		stringstream strm;
		strm.write(pBuffer, size);
		string s;
        
        while(getline(strm,s,'\r'))
		{
            std::string::size_type p_r = s.find_last_of('\r');
            if(p_r != std::string::npos) s.erase(p_r,p_r + 1);
            
            std::string::size_type p_n = s.find_last_of('\n');
            if(p_n != std::string::npos) s.erase(p_n,p_n + 1);
            
            if(s.empty())
                continue;
			vector<string> field;
			split(&field,s);
			_datas.push_back(field);
			_colNum=MAX(_colNum,field.size());
		}
		delete[] pBuffer;
		return true;
	}
	return false;
}

void CSVParse::split(vector<string>* field,string line)
{
    string fld;
    long i, j;
    
    if (line.length() == 0)
        return ;
    i = 0;
    
    do
    {
        if (i < line.length() && line[i] == '"')
            j = advquoted(line, &fld, i+1);    // skip quote
        else
            j = advplain(line, &fld, i);
        
        field->push_back(fld);
        i = j + 1;
    } while (j < line.length());
}

// advquoted: quoted field; return index of next separator
long CSVParse::advquoted(const string& s, string* splitStr, long i)
{
    (*splitStr) = "\0";
    
    long k = s.find_first_of('"', i);
    if(k == string::npos)
    {
        return i - 1;
    }
    else
    {
        (*splitStr)  = string(s, i, k-i);
    }
    
    return k + 1;
}

// advplain: unquoted field; return index of next separator
long CSVParse::advplain(const string& s, string* splitStr, long i)
{
    long j;
    
    j = s.find_first_of(_fieldSeparator, i); // look for separator
    if (j > s.length())               // none found
    {
        j = s.length();
    }
    (*splitStr) = string(s, i, j-i);
    return j;
}


const char* CSVParse::getDatas( unsigned int rows,const char* colsName )
{
	return getData(rows, findColsData(0,colsName) );
}

int  CSVParse::findColsData(unsigned int rows,const char* value)
{
	if ( rows < _datas.size() )
	{
		long size = _datas[rows].size();
		for ( unsigned int i = 0; i <size;i++ )
		{
			if( _datas[0][i].compare(value) == 0 )
			{
				return i;
			}
		}
	}
	return -1;
}

// getfield: return n-th field
const char* CSVParse::getData(unsigned int rows,unsigned int cols)
{
    if (rows>=_datas.size()|| cols>=_datas[rows].size())
    {
        return "\0";
    }
    
    return _datas[rows][cols].c_str();
}