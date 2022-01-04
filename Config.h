#pragma once
#define COMMENT_CHAR '#'
#include<string>
#include<map>
#include<fstream>
#include<iostream>
#include<math.h>
#include <QDebug>
#include <memory>

using namespace std;

void LOG(const char* str, const char* file);

class ConfigSearchPeak
{
public:
	ConfigSearchPeak(int startPoint, int ch1Th, int ch2Th, int ch3Th,int FBGInterval) {
		 m_startPoint = startPoint;
		 m_ch1ThredHold = ch1Th;
		 m_ch2ThredHold = ch2Th;
		 m_ch3ThredHold = ch3Th;
         m_FBGInterval = (int)ceil(FBGInterval/5);
    }
public:
	int m_startPoint;
	int m_ch1ThredHold;
	int m_ch2ThredHold;
	int m_ch3ThredHold;
	int m_FBGInterval;
};

class ConfigDemodulation
{
public:
    ConfigDemodulation(string CacPhase, string filt, string lpfilter, string unwrap)
    : m_CacPhase(CacPhase),
      m_Filt(filt),
      m_Unwrap(unwrap),
      m_LpFilter(lpfilter)
    {}
    bool getUnwrap() const
    {
        return m_Unwrap == "true" ? true : false;
    }

    bool getFilter() const
    {
        return m_Filt == "true" ? true : false;
    }

    void setPeakNum(int peakNum){
        m_peakNum = peakNum;
    }

    int getPeakNum() const
    {
        return m_peakNum;
    }

    void setIP(string ip)
    {
        IP = ip;
    }
    void setPort(int port){
        Port = port;
    }

    bool getCacPhase() const
    {
        return m_CacPhase == "true" ? true: false;
    }

public:
    string m_CacPhase;
    string m_Filt;
    string m_LpFilter;
    string m_Unwrap;

    int m_peakNum;

    string IP;
    int Port;


};

class ConfigFilter
{
    public:
        ConfigFilter(string HighPassCutoff, string LowPassCutoff)
        {
            setCutOff(HighPassCutoff, LowPassCutoff);
        }
        
        void setCutOff(string HighPassCutoff, string LowPassCutoff)
        {
            m_hpcutoff = HighPassCutoff;
            m_lpcutoff = LowPassCutoff;
        }

    public:
        string m_hpcutoff;
        string m_lpcutoff;
};

class ConfigFPGACard
{
public:
    ConfigFPGACard(int portName,
                   int baudRate,
                   int freq,
                   int channelLen,
                   int packagePerRead = 10,
                   int channelCount = 3,
                   int bitCount = 16) {
		m_baudRate = baudRate;
		m_portName = portName;
		m_freq = freq;
		m_channelLen = channelLen;
		m_channelCount = channelCount;
		m_bitCount = bitCount;
        m_packagePerRead = packagePerRead;
    }
    void setParam(int portName, int baudRate, int freq, int channelLen, int channelCount = 3, int bitCount = 16)
	{
		m_baudRate = baudRate;
		m_portName = portName;
		m_freq = freq;
		m_channelLen = channelLen;
		m_channelCount = channelCount;
		m_bitCount = bitCount;
    }
public:
	int m_portName;
	int m_baudRate;
	int m_freq;
	int m_channelCount;
	int m_channelLen;
	int m_bitCount;
    int m_packagePerRead;
};

class ConfigDataProcess
{
public:

ConfigDataProcess(string path,
             bool isSave,
             uint16_t wavelength,
             float ValueMax,
             float ValueMin,
             bool send)
    {
        m_path = path;
        m_isSave = isSave;
        m_WaveFormLength = wavelength;
        m_ValueMax = ValueMax;
        m_ValueMin = ValueMin;
        m_isSend = send;
    }
void setParam(string path,
              bool isSave,
              uint16_t wavelength,
              float ValueMax,
              float ValueMin,
              bool send)
    {
    m_path = path;
    m_isSave = isSave;
    m_WaveFormLength = wavelength;
    m_ValueMax = ValueMax;
    m_ValueMin = ValueMin;
    m_isSend = send;
    }



public:
    string m_path;
    bool m_isSave;
    bool m_isSend;
    uint16_t m_WaveFormLength;
    float m_ValueMax;
    float m_ValueMin;
};

class ConfigWave
{
public:
    ConfigWave() = default;
    ConfigWave(const bool& iswave):
        m_isWave(iswave)
    {}

    ~ConfigWave()
    {}

    bool getIsWave() const
    {
        return m_isWave;
    }

private:
    bool m_isWave;
};


class ConfigProgram
{
public:
    ConfigProgram(bool Debugmode, int peaknum)
    {
        m_debugmode = Debugmode;
        m_peaknum = peaknum;
    }

    bool ignoreDevice()
    {
        qDebug() << m_debugmode << endl;
        return m_debugmode;
    }


public:
    bool m_debugmode;
    int m_peaknum;
};



class Config {

public:
    Config() = default;
    ~Config()
    {
        delete m_FPGACard;
        delete m_Peak;
        delete m_demodulation;
        delete m_DataProcess;
        delete m_Program;
    }

    void deleteSpace(string &str);
    void deleteComment(string &str);
	string read2string(string FileName);

	bool get_word_bool(std::string &str, std::string name);
	std::string get_word_type(std::string &str, std::string name);
	float get_word_float(std::string &str, std::string name);
	int get_word_int(std::string &str, std::string name);

	void Init(string path);

	void setPath(string path) {
		m_path = path;
    }

	void printConfigs();

    void set_debugpath(string path)
    {
        m_debugpath = path;
    }

    void InitDebug(string debugpath);

public:
	std::string m_configStr;
    std::string m_debugstr;
	string m_path;
    string m_debugpath;

	ConfigFPGACard *m_FPGACard;
	ConfigSearchPeak *m_Peak;
	ConfigDemodulation *m_demodulation;
    ConfigFilter *m_Filter;
    ConfigDataProcess *m_DataProcess;
    shared_ptr<ConfigWave> m_Wavesp;
    ConfigProgram *m_Program;

};

class GetConfig
{

public:
    GetConfig() = default;
    GetConfig(const Config& cfg):
        myConfig(cfg)
    {
        portName = cfg.m_FPGACard->m_portName;
        baudRate = cfg.m_FPGACard->m_baudRate;
        fre = cfg.m_FPGACard->m_freq;
        channelLength = cfg.m_FPGACard->m_channelLen;
        channelCount = cfg.m_FPGACard->m_channelCount;
        bitCount = cfg.m_FPGACard->m_bitCount;
        packagePerRead = cfg.m_FPGACard->m_packagePerRead;

        calcPhase = String_to_bool(cfg.m_demodulation->m_CacPhase);
        isFilter = String_to_bool(cfg.m_demodulation->m_Filt);
        unWrap = String_to_bool(cfg.m_demodulation->m_Unwrap);
        lpFilter = String_to_bool(cfg.m_demodulation->m_LpFilter);

        hpCutOff = cfg.m_Filter->m_hpcutoff;
        lpCutoff = cfg.m_Filter->m_lpcutoff;

        deBug = cfg.m_Program->ignoreDevice();
    }

    ~GetConfig() {}

    Config myConfig;

    const Config* getConstPointoConfig() const
    {
        return &myConfig;
    }

    Config* getPointoConfig()
    {
        return &myConfig;
    }

    bool getConfig_calcPhase() const
    {
        return calcPhase;
    }

    bool getConfig_Filter() const
    {
        return isFilter;
    }

    bool getConfig_unWrap() const
    {
        return unWrap;
    }

    bool getConfig_LpFilter() const
    {
        return lpFilter;
    }


    bool getConfig_Debug() const
    {
        return deBug;
    }

    int getConfig_freqency() const
    {
        return fre;
    }

    int getConfig_portName() const
    {
        return portName;
    }

    int getConfig_bitCount() const
    {
        return bitCount;
    }

    int getConfig_channelCount() const
    {
        return channelCount;
    }

    int getConfig_channelLength() const
    {
        return channelLength;
    }

    int getConfig_packagePerRead() const
    {
        return packagePerRead;
    }

    int getPeakNum() const
    {
        return myConfig.m_demodulation->getPeakNum();
    }

    const string& getConfig_hpCutOff() const
    {
        return hpCutOff;
    }

    const string& getConfig_lpCutOff() const
    {
        return lpCutoff;
    }

private:
    bool String_to_bool(const string& str) const
    {
        if(str == string("true"))
            return true;

        return false;
    }

    int portName;
    int baudRate;
    int fre;
    int channelLength;
    int channelCount;
    int bitCount;
    int packagePerRead;
    bool calcPhase;
    bool isFilter;
    bool unWrap;
    bool lpFilter;

    string hpCutOff;
    string lpCutoff;

    bool deBug;
};

