#pragma once
#include "Config.h"
#include "USBCtrl.h"
#include "WzSerialPort.h"
#include <QThread>

#define  USB_FAILED  WM_USER+11
#define  SERIAL_WRITE_CONFIG_COMMAND_FAILED WM_USER+21
#define  SERIAL_WRITE_CONFIG_COMMAND_FINISHED WM_USER+22
#define  SERIAL_WRITE_PEAKPOS_FAILED  WM_USER+23

#define  SEARCH_PEAK_FINISHED WM_USER+31


class PeakSearch: public QThread
{
    Q_OBJECT
public:
	PeakSearch();
    ~PeakSearch();
	
	void data_calibration(unsigned short step, unsigned short* &data_need_to_be_processed);
	
    void SetParam(Config *cfig);
	void SetParam(unsigned int startPoint,
		unsigned int ch1TH,
		unsigned int ch2TH,
		unsigned int ch3TH,
		unsigned int FBGIntvl,
		unsigned int lenOfChannel);

	void SavePeak2Bin(char *file, unsigned int peakNum, int *pos, float *posValue, float *ALLData);
	int findPeaks(float*data,int *peakPos, float*peakValue, unsigned int startPoint, unsigned int threadHold, unsigned int interval);

    void savePeak2Txt(char *file, unsigned int peakNum, int *pos);

    void readTxt2Peak();

	void Init(Config *cfig,HWND hWnd);

    void run();

    void closeDevice();

    void ConfigDevice();

    int SendPeakPos(int Channel);

    void savePeakNum(Config *cfig)
    {
        cfig->m_demodulation->setPeakNum(peakNum[0]);
    }

    void savePeakNum(Config* cfig, bool debug)
    {
        if(debug)
        {
            cfig->m_demodulation->setPeakNum(cfig->m_Program->m_peaknum);
        }
        else
        {
            cfig->m_demodulation->setPeakNum(peakNum[0]);
        }
    }

	void setHWND(HWND hWnd)
	{
		m_hWnd = hWnd;
	}


public:
	USBCtrl *USB;
	WzSerialPort *Series;
	HWND m_hWnd;
    bool cfigSuc;
public:
	int m_startPoint;
	unsigned int m_ch1TreadHold;
	unsigned int m_ch2TreadHold;
	unsigned int m_ch3TreadHold;
	unsigned int m_FBGInterval;
	unsigned int m_lenOfChannel;


	unsigned int peakNum[3];
	int *m_peakPos[3];
	float *m_peakValue[3];

	unsigned short *CH1Data;
	unsigned short *CH2Data;
	unsigned short *CH3Data;

	float *CH1DataAdd;
	float *CH2DataAdd;
	float *CH3DataAdd;

	BYTE* RECORD_BUF;


};




