#include "stdafx.h"
#include "ConfigFile.h"
#include <fstream>

ConfigFile::ConfigFile(const std::string& fileName)
{
    FilePathName=fileName;
    std::ifstream file(fileName.c_str());
    std::string line,key,value,insection;
    int posEqual;

    while(std::getline(file,line))
    {
        line =trim(line);// beside space enter table ..
        for(int i=0;i<line.length();i++)
        {
            if(line[i]=='#')
            {
                line.erase(i,line.length());
                break;
            }
        }
        if(!line.length())continue;
        if(line[0]=='[')
        {
            insection=trim(line.substr(1,line.find(']')-1));
            continue;
        }

        posEqual = line.find('=');
        key = trim(line.substr(0,posEqual));
        value = trim(line.substr(posEqual+1));
        m_content[std::string((insection+'/'+key).c_str())]=std::string(value.c_str());
    }
    file.close();
}

std::string ConfigFile::trim(const std::string &source, const char *delims)
{
    std::string res(source);
    res = makeLower(res);
    std::string::size_type index = res.find_last_not_of(delims);
    if(index !=std::string::npos)
    {
        res.erase(++index);
    }
    index = res.find_first_not_of(delims);
    if(index !=std::string::npos)
    {
        res.erase(0,index);
    }
    else
    {
        res.erase();
    }
    return res;
}

std::string ConfigFile::getValue(const std::string &key,const std::string& section)
{

    std::map<std::string,std::string>::const_iterator constIter=m_content.find(makeLower(section+'/'+key));
    if(constIter ==m_content.end())
    {
        return "";
    }
    return constIter->second;
}

std::string ConfigFile::makeLower(const std::string& str)
{
    std::string s = str;
    for(std::string::iterator iter =s.begin();iter!=s.end();++iter)
    {
        if(tolower(*iter) != (int)*iter)// beside number
        {
            *iter = (char)tolower(*iter);
        }
    }
    return s;
}

void ConfigFile::addValue(const std::string &key,const std::string value,const std::string& section)
{
    m_content[makeLower(section+'/'+key)]=value;
}
void ConfigFile::removeValue(const std::string &key,const std::string& section)
{
    m_content.erase(makeLower(section+'/'+key));
}
void ConfigFile::save()
{
    std::ofstream file(FilePathName.c_str());
    std::string currentSection="";
    for(std::map<std::string,std::string>::iterator i=m_content.begin();i!=m_content.end();i++)
    {
        std::string section=i->first.substr(0, i->first.find_first_of('/'));
        std::string key=i->first.substr(i->first.find_first_of('/')+1);
        if(section==currentSection)
        {
            file<<key<<"="<<i->second<<std::endl;
        }
        else
        {
            currentSection = section;
            file<<"["<<makeLower(section)<<"]"<<std::endl;
            file<<key<<"="<<i->second<<std::endl;

        }

    }
    file.close();

}
