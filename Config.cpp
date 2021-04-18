#include "config.h"
#include<fstream>
#include<iostream>
#include<string>
#include<map>
#include<QDebug>


using namespace std;

void LOG(const char* str, const char* file)
{
	FILE* pFile;
	if (fopen_s(&pFile,file,"a"))
	{
		printf("Can't Open file: %s\n", file);
	}
	printf("%s", str);
	fprintf(pFile, "%s", str);
	fclose(pFile);
}
void Config::deleteSpace() {
	if (m_configStr.empty())
		return;
	size_t pos1, pos2, e, t, n;
	while (1) {
		e = m_configStr.find(' ');
		t = m_configStr.find('\t');
		n = m_configStr.find('\n');
		if (e == std::string::npos && n == std::string::npos && t == std::string::npos)break;
		if (e<t || t == std::string::npos)pos1 = e;
		else pos1 = t;
		if (n < pos1 || pos1 == std::string::npos)pos1 = n;
		for (pos2 = pos1 + 1; pos2 < m_configStr.size(); pos2++) {
			if (!(m_configStr[pos2] == '\t' || m_configStr[pos2] == '\n'
				|| m_configStr[pos2] == ' '))
				break;
		}
		m_configStr.erase(pos1, pos2 - pos1);
	}

}


void Config::deleteComment() {
	size_t pos1, pos2;
	while (1) {
		pos1 = m_configStr.find("#");
		if (pos1 == std::string::npos)
			break;
		for (pos2 = pos1 + 1; pos2 < m_configStr.size(); pos2++) {
			if (m_configStr[pos2] == '#')
				break;
		}
		m_configStr.erase(pos1, pos2 - pos1 + 1);
	}
}


string
Config::read2string(string FileName) {
	char *pBuf;
	FILE *pFile = NULL;
	char logStr[1025];
	if (fopen_s(&pFile, FileName.c_str(),"r")) {
        sprintf(logStr, "Can not find this file.");
        LOG(logStr, "C:/DAS/log.txt");
        return 0;
	}
	//move pointer to the end of the file
	fseek(pFile, 0, SEEK_END);
	//Gets the current position of a file pointer.offset 
	size_t len = ftell(pFile);
	pBuf = new char[len];
	//Repositions the file pointer to the beginning of a file
	rewind(pFile);
	if (fread(pBuf, 1, len, pFile) != len) {
		//LOG("fread fail", "result/log.txt");
	}
	fclose(pFile);
	string res = pBuf;
	return res;
}

bool Config::get_word_bool(string &str, string name) {
	size_t pos = str.find(name + "=");

	char  logStr[256];
	if (pos == string::npos) {
		sprintf(logStr, "Can't find %s \n", name.c_str());
        LOG(logStr, "C:/DAS/log.txt");
	}

    unsigned int i = pos + 1;
	bool res = true;
	while (1) {
		if (i == str.length()) break;
		if (str[i] == ';') break;
		++i;
	}
	string sub = str.substr(pos, i - pos + 1);
	if (sub[sub.length() - 1] == ';') {
		string content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
		if (!content.compare("true")) res = true;
		else res = false;
	}
	str.erase(pos, i - pos + 1);
	return res;
}

int Config::get_word_int(string &str, string name) {
	size_t pos = str.find(name);
	char  logStr[256] = {0};
	if (pos == string::npos){
		sprintf(logStr,"Can't find %s \n",name.c_str());
		LOG(logStr, "C:/DAS/log.txt");
		return -1;
	}
    unsigned int i = pos + 1;
	int res = 1;
	while (1) {
		if (i == str.length()) break;
		if (str[i] == ';') break;
		++i;
	}
	string sub = str.substr(pos, i - pos + 1);
	if (sub[sub.length() - 1] == ';') {
		string content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
		res = atoi(content.c_str());
	}
	str.erase(pos, i - pos + 1);
	return res;
}

float Config::get_word_float(string &str, string name) {
	size_t pos = str.find(name + "=");

	char  logStr[256] = { 0 };
	if (pos == string::npos) {
		sprintf(logStr, "Can't find %s \n", name.c_str());
        LOG(logStr, "C:/DAS/log.txt");
        return -1.0;
	}

    unsigned int i = pos + 1;
	float res = 0.0f;
	while (1) {
		if (i == str.length()) break;
		if (str[i] == ';') break;
		++i;
	}
	string sub = str.substr(pos, i - pos + 1);
	if (sub[sub.length() - 1] == ';') {
		string content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
		res = atof(content.c_str());
	}
	str.erase(pos, i - pos + 1);
	return res;
}


string Config::get_word_type(string &str, string name) {
	size_t pos = str.find(name + "=");
	char  logStr[256] = { 0 };
	if (pos == string::npos) {
		sprintf(logStr, "Can't find %s \n", name.c_str());
        LOG(logStr, "C:/DAS/log.txt");
	}
	if (pos == str.npos) {
		return "NULL";
	}

    unsigned int i = pos + 1;
	while (1) {
		if (i == str.length()) break;
		if (str[i] == ';') break;
		++i;
	}
	string sub = str.substr(pos, i - pos + 1);
	string content;
	if (sub[sub.length() - 1] == ';') {
		content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
	}
	str.erase(pos, i - pos + 1);
	return content;
}

void Config::Init(string path) {
	setPath(path);
	m_configStr = read2string(m_path);

	deleteSpace();
	deleteComment();
	
	int baud = get_word_int(m_configStr, "BaudRate");
	int portName = get_word_int(m_configStr, "PortName");
	int freq = get_word_int(m_configStr, "Freqency");
	int chanLen = get_word_int(m_configStr, "ChannelLength");
    int channelCount = get_word_int(m_configStr, "ChannelCount");
    int bitCount = get_word_int(m_configStr, "BitCount");
    int packageRead = get_word_int(m_configStr,"PackagePerRead");

    m_FPGACard = new ConfigFPGACard(portName, baud, freq, chanLen,
                                    packageRead,channelCount, bitCount);

	int startpoint = get_word_int(m_configStr, "StartPoint");
	int ch1Th = get_word_int(m_configStr, "CH1ThreadHold");
	int ch2Th = get_word_int(m_configStr, "CH2ThreadHold");
	int ch3Th = get_word_int(m_configStr, "CH3ThreadHold");
    int FBGInter = get_word_int(m_configStr,"FBGInterval");
    m_Peak = new ConfigSearchPeak(startpoint, ch1Th, ch2Th, ch3Th, FBGInter);

    string calphase = get_word_type(m_configStr,"CaculationPhase");
    string filt = get_word_type(m_configStr,"Filter");
    string unwrap = get_word_type(m_configStr,"UnWrap");
    string unwrapFilt = get_word_type(m_configStr,"UnWrapFilter");

    m_demodulation = new ConfigDemodulation(calphase,filt,unwrapFilt,unwrap);
    string AddressIP = get_word_type(m_configStr,"IP");

    m_demodulation->setIP(AddressIP);
    int IPPort = get_word_int(m_configStr,"PORT");
    m_demodulation->setPort(IPPort);

    string savepath = get_word_type(m_configStr,"SavePath");
    bool isSave = get_word_bool(m_configStr,"IsSave");
    int wavelen = get_word_int(m_configStr,"WaveLength");
    float ValueMin = get_word_float(m_configStr,"ValueMin");
    float ValueMax = get_word_float(m_configStr,"ValueMax");
    bool send = get_word_bool(m_configStr,"IsSend");
    m_DataProcess = new ConfigDataProcess(savepath,isSave,wavelen,ValueMax,ValueMin,send);

}

void Config::printConfigs() {
	printf("BaudRate = %d", m_FPGACard->m_baudRate);
	printf("COM = %d", m_FPGACard->m_portName);
	printf("startpoint = %d", m_Peak->m_startPoint);
}
