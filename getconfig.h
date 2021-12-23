#ifndef GETCONFIG_H
#define GETCONFIG_H
#include <iostream>
#include "Config.h"
#include <string>

using namespace std;

class GetConfig
{

public:
    GetConfig() = default;
    GetConfig(const Config& cfg):
        myConfig(cfg)
    {
        calcPhase = String_to_bool(cfg.m_demodulation->m_CacPhase);
        isFilter = String_to_bool(cfg.m_demodulation->m_Filt);
        unWrap = String_to_bool(cfg.m_demodulation->m_Unwrap);
        lpFilter = String_to_bool(cfg.m_demodulation->m_LpFilter);


        deBug = cfg.m_Program->ignoreDevice();
    }

    ~GetConfig() {}

    bool getConfig_calcPhase()
    {
        return calcPhase;
    }


    bool getDebug()
    {
        return deBug;
    }


private:
    bool String_to_bool(const string& str) const
    {
        if(str == string("true"))
            return true;

        return false;
    }
    Config myConfig;
    bool calcPhase;
    bool isFilter;
    bool unWrap;
    bool lpFilter;


    bool deBug;
};



#endif // GETCONFIG_H
