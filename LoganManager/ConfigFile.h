#ifndef CONFIGFILE_H
#define CONFIGFILE_H

/********************************************************
 *
 *   Read Configure file in STL, if no section you can set it
 * empty as "" it can read INI file or linux conf file.
 *
 * Author: Yoohoo Niu     mail: wand_niu@live.com
 *
 *
 ********************************************************/

#include "stdafx.h"
#include <iostream>
#include <map>

class ConfigFile
{
public:
    ConfigFile(const std::string& fileName);
    std::string getValue(const std::string &key,const std::string& section="");
    void addValue(const std::string &key,const std::string value,const std::string& section="");
    void removeValue(const std::string &key,const std::string& section="");
    void save();
private:
    std::map<std::string,std::string> m_content;
    std::string FilePathName;
    std::string trim(const std::string &source,const char* delims=" \t\r\n");
    std::string makeLower(const std::string& str);

};
#endif // CONFIGFILE_H



/**
 *
 *
 *    file: A.conf
 * ==>[Name]
 *     AppName=Yoohoo App
 *     TitleName=test
 *    [Goods]
 *     Color=red
 *     Handle=yes
 *
 * ==============================
 *
 * ConfigFile cf("A.conf");
 *
 * cout<<cf.getValue("Handle","Goods")<<endl;
 *
 * ==>yes
 *
 *
 *    file: B.conf
 * ==> AppName=Yoohoo App
 *     TitleName=test
 *     Color=red
 *     Handle=yes
 *
 * ==============================
 *
 * ConfigFile cf("B.conf");
 *
 * cout<<cf.getValue("Handle","")<<endl;
 *
 * ==>yes
 *
 */
