#pragma once
#define COMMENT_CHAR '#'
#include<string>
#include<map>
#include<fstream>
#include<iostream>
#include<math.h>
#include <QDebug>

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
    ConfigDemodulation(string CacPhase, string filt, string lpfilter, string unwrap){
        SetParam(CacPhase, filt, lpfilter, unwrap);
    }
    void SetParam(string CacPhase, string filt, string lpfilter, string unwrap)
    {
        m_CacPhase = CacPhase;
        m_Filt = filt;
        m_Unwrap = unwrap;
        m_LpFilter = lpfilter;
    }
    void setPeakNum(int peakNum){
        m_peakNum = peakNum;
    }

    int getPeakNum()
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
	static Config* instance() {
		static Config* config = new Config();
        return config;
	}
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
    ConfigProgram *m_Program;

};


