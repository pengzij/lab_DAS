#pragma once
#include"Config.h"
#include <Windows.h>
#include "CU3AD.h"
class USBCtrl
{
public:
    USBCtrl(const Config *cfig);
    USBCtrl();
    ~USBCtrl();
public:
	int USBStart();
	int USBClose();
    void setParam(int chanCount, int Freq, int bitCount, int lenOfChannal,int pakgPerRead);
    void setParam(const Config *cfig);
    void setParam(shared_ptr<GetConfig>& PSgcfg);

	int getDataFromUSB(BYTE* recvBuf);

	void setpackagePerRead(int size);
	void setLenOfChannel(int len) {
		m_LenofChannels = len;
	}

public:
	HANDLE usb_dev;
	int m_ChannelCount;
	int m_bitCount;
	int m_LenofChannels;
	int m_Freq;
    int packagePerRead;

};

