#include "PeakSearch.h"
#include <stdio.h>
#include <fstream>
#include <QDebug>
#include <iostream>
#include <string>
#include <QMessageBox>

PeakSearch::PeakSearch()
{
}

PeakSearch::~PeakSearch()
{
	for (size_t i = 0; i < 3; i++)
	{
		delete[] m_peakPos[i];
		delete[] m_peakValue[i];
	}

    closeDevice();

	delete USB;
	delete Series;

	delete[] CH1Data;
	delete[] CH2Data;
	delete[] CH3Data;
	delete[] CH1DataAdd;
	delete[] CH2DataAdd;
	delete[] CH3DataAdd;
	delete[] RECORD_BUF;
}

void PeakSearch::data_calibration(unsigned short step, unsigned short* &data_need_to_be_processed)
{
    unsigned short* bg = new unsigned short[m_lenOfChannel]();
	//bg = (unsigned short*)malloc(sizeof(unsigned short) * (m_lenOfChannel - 4));
	bg[0] = data_need_to_be_processed[0];
    for (size_t i = 1; i < (m_lenOfChannel); i++)
	{
		bg[i] = data_need_to_be_processed[i]<(bg[i - 1] + step) ? data_need_to_be_processed[i] : (bg[i - 1] + step);
		data_need_to_be_processed[i] = (unsigned short)(data_need_to_be_processed[i] - bg[i]);
	}
	data_need_to_be_processed[0] = 0;
	delete[] bg;
}


void PeakSearch::SetParam(Config *cfig)
{
    m_ch1TreadHold = cfig->m_Peak->m_ch1ThredHold;
    m_ch2TreadHold = cfig->m_Peak->m_ch2ThredHold;
    m_ch3TreadHold = cfig->m_Peak->m_ch3ThredHold;
    m_startPoint = cfig->m_Peak->m_startPoint;

    m_lenOfChannel = cfig->m_FPGACard->m_channelLen;

    m_FBGInterval = cfig->m_Peak->m_FBGInterval;
}


void PeakSearch::SetParam(
	unsigned int startPoint,
	unsigned int ch1TH,
	unsigned int ch2TH,
	unsigned int ch3TH,
	unsigned int FBGIntvl,
	unsigned int lenOfChannel)
{
	m_ch1TreadHold = ch1TH;
	m_ch2TreadHold = ch2TH;
	m_ch3TreadHold = ch3TH;
	m_startPoint = startPoint;

	m_lenOfChannel = lenOfChannel;

	m_FBGInterval = FBGIntvl;
}

int PeakSearch::findPeaks(
	float *data, 
	int *peakPos, 
	float*peakValue, 
	unsigned int startPoint, 
	unsigned int threadHold, 
	unsigned int interval) 
{
	int peakNum = 0;
	int priorPeakPos = startPoint;
	float priorPeakValue = 0;
//    qDebug()<<"startPoint = " << startPoint;
//    qDebug()<<"m_lenOfChannel = "<< m_lenOfChannel;
    qDebug()<<"interval = " << interval;
    for (size_t i = startPoint; i < m_lenOfChannel; i++)
	{
        //qDebug()<<"findPeak data = "<<data[i];
		if ((data[i] > data[i - 1]) && (data[i]) > data[i + 1])
		{
			if (data[i] > threadHold)
			{
				if (i - priorPeakPos >= interval)
				{
					peakPos[peakNum] = i;
					priorPeakPos = i;
					peakValue[peakNum] = data[i];
					priorPeakValue = (float)data[i];
					peakNum++;
				}
				else 
				{
					if (data[i] > priorPeakValue )
					{
						peakPos[peakNum - 1] = i;
						priorPeakPos = i;
						peakValue[peakNum - 1] = data[i];
						priorPeakValue = data[i];
					}
				}
			}
		}
	}

    for(size_t x = 0;x < peakNum; x++)
    {
        int peak_p = peakPos[x] - ceil(interval/2) - 1;
        int peak_b = peakPos[x] + ceil(interval/2) + 1;
        int half_p = peakPos[x];
        int half_b = peakPos[x];
        float half = peakValue[x]/2;
        for(size_t y = peak_p;y <= peak_b;y++)
        {
            if(data[y-1] <= half && data[y+1] >=half)
                half_p = y;
            if(data[y-1] >= half && data[y+1] <=half)
                half_b = y;
        }
        peakPos[x] = half_p + abs(ceil((half_b - half_p)/2.0));
        peakValue[x] = data[peakPos[x]];
    }

	return peakNum;
}

void PeakSearch::SavePeak2Bin(char *file,unsigned int peakNum, int *pos, float *posValue, float *ALLData)
{
	FILE *pFile;
	char  logStr[256] = { 0 };
	if (fopen_s(&pFile, file, "wb"))
	{
		sprintf(logStr, "PeakSearch Open Save Peak File: %s Faied \n",file);
        LOG(logStr, "C:/DAS/log.txt");
		return;
	}
	fwrite(&peakNum, sizeof(unsigned int ), 1, pFile);
    fwrite(&m_lenOfChannel,sizeof(unsigned int), 1,pFile);
	fwrite(pos, sizeof(int), peakNum, pFile);
	fwrite(posValue, sizeof(float), peakNum, pFile);
	fwrite(ALLData, sizeof(float), m_lenOfChannel, pFile);

	fclose(pFile);

}

void PeakSearch::savePeak2Txt(char *file, unsigned int peakNum, int *pos)
{
    FILE* pFile;
    char  logStr[256] = { 0 };
    if (fopen_s(&pFile, file, "w"))
    {
        sprintf(logStr, "PeakSearch Open Save Peak File: %s Faied \n",file);
        LOG(logStr, "C:/DAS/log.txt");
        return;
    }
    for(size_t i = 0; i<peakNum; i++)
    {
        fprintf(pFile,"%d\n",pos[i]);
    }
    fclose(pFile);
}

void PeakSearch::readTxt2Peak()
{
    string filepath = "C:/DAS/peak.txt";
    int j = 0;
    for (int i = 0;i < 3;i++)
    {
        ifstream in;//ifstream定义了一个输入流in(文件流)，它被初始化从文件中读取数据
        in.open(filepath);
        if (in)//检查文件的读取是否成功,养成良好的习惯！
        {
            string buf;
            while (getline(in,buf))//每行独立输入的方法，利用getline
            {
                m_peakPos[i][j] = stoi(buf);
                j++;
            }
        }
        peakNum[i] = j;
        j = 0;
    }
    for (int i=0;i < peakNum[0];i++)
    {
        qDebug() << "peak1_" << i << "= " << m_peakPos[0][i]<<endl;
    }
}

void PeakSearch::Init(Config *cfig,HWND hWnd)
{
	m_hWnd = hWnd;

	USB = new USBCtrl();
    USB->setParam(cfig);//

	Series = new WzSerialPort();
	
	Series->Init(cfig);

    SetParam(cfig);

	for (size_t i = 0; i < 3; i++)
	{
		m_peakPos[i] = new int[1024]();
		m_peakValue[i] = new float[1024]();
	}

	int len = USB->m_LenofChannels;
    qDebug()<<"CH1Data len = " << len;
	CH1Data = new unsigned short[len]();
	CH1DataAdd = new float[len]();

	CH2Data = new unsigned short[len]();
	CH2DataAdd =new float[len]();
	
    CH3Data = new unsigned short[len]();
    CH3DataAdd = new float[len]();


	long LEN = USB->packagePerRead * USB->m_LenofChannels * 3 * USB->m_bitCount / 8;
    qDebug()<<"RECORD_BUF len = "<< LEN;
	RECORD_BUF = new BYTE[LEN]();
}

int PeakSearch::SendPeakPos(int Channel)
{
    int hasWrite = Series->WriteCommand(4, m_peakPos[Channel], peakNum[Channel],Channel);

    return hasWrite;
}

void PeakSearch::ConfigDevice()
{
    bool serialSuc = false;
    bool USBSuc = true;
    while(1){
        Series->WriteCommand(2);
        Sleep(100);
        if(Series->isSendCommandSuccessful(2)) {
            serialSuc = true;
            PostMessage(m_hWnd, SERIAL_WRITE_CONFIG_COMMAND_FINISHED, 0, 0);
            break;

        }
        else{
            char logStr[256] = { 0 };
            sprintf(logStr, "Write Serial Config Order failed! \n");
            LOG(logStr, "C:/DAS/log.txt");
            qDebug()<<"Write Serial Config Order failed! \n"<<endl;
            //PostMessage(m_hWnd, SERIAL_WRITE_CONFIG_COMMAND_FAILED, 0, 0);
        }
    }
    Sleep(10);
    Series->WriteCommand(3);

    cfigSuc = serialSuc && USBSuc;
}


void PeakSearch::run()
{
    qDebug() << "start PeakSearch" << endl;
    int usb = USB->USBStart();
    USB->USBClose();

    usb = USB->USBStart();
    if(usb != -1)
    {
        PostMessage(m_hWnd,USB_FAILED,0,0);
        return;
    }

    if(cfigSuc)
    {
        int Count_10000 = 0;
        while (Count_10000 < 10000)
        {
            int hasRecv = USB->getDataFromUSB(RECORD_BUF);
            //qDebug()<<"hasRecv = " << hasRecv;
            //hasRecv = 0;
            //qDebug()<<"m_bitCount = "<<USB->m_bitCount;

            //qDebug()<<"USB->m_ChannelCount" <<USB->m_ChannelCount;
            if (hasRecv > 0)
            {
                int num = 0;
                unsigned long j = 0;
                unsigned long i = 0;
                  for (j = 0; j < hasRecv; j++)
                  {
                      if((USB->m_bitCount/8) == 2)
                      {
                            for (i = j * USB->m_LenofChannels * USB->m_ChannelCount * 2;
                                i < USB->m_LenofChannels * 2 + j * USB->m_ChannelCount * USB->m_LenofChannels * 2;
                                i += 2)
                            {                               
                                num = (i - j * USB->m_ChannelCount * USB->m_LenofChannels * 2) / 2;

                                CH1Data[num] = RECORD_BUF[i + 1];
                                CH1Data[num] = ((CH1Data[num] & 0x3f) << 8) + (RECORD_BUF[i]);
                                //qDebug()<<"CH1Data["<<num<<"]="<<CH1Data[num];
                                CH2Data[num] = RECORD_BUF[i + USB->m_LenofChannels * 2 + 1];
                                CH2Data[num] = ((CH2Data[num] & 0x3f) << 8) + (RECORD_BUF[i + USB->m_LenofChannels * 2]);
                                //qDebug()<<"CH2Data["<<num<<"]="<<CH2Data[num];
                                CH3Data[num] = RECORD_BUF[i + USB->m_LenofChannels * 4 + 1];
                                CH3Data[num] = ((CH3Data[num] & 0x3f) << 8) + (RECORD_BUF[i + USB->m_LenofChannels * 4]);
                                //qDebug()<<"CH3Data["<<num<<"]="<<CH3Data[num];
                            }
                        }
                        else
                        {
                            for (int i = j * USB->m_LenofChannels * USB->m_ChannelCount;
                                i < (USB->m_LenofChannels + j * USB->m_ChannelCount * USB->m_LenofChannels);
                                i += 1)
                            {
                                num = (i - j * USB->m_ChannelCount * USB->m_LenofChannels);
                                CH1Data[num] = RECORD_BUF[i + 1];
                                CH2Data[num] = RECORD_BUF[i + USB->m_LenofChannels + 1];
                                CH3Data[num] = RECORD_BUF[i + USB->m_LenofChannels * 2 + 1];
                            }
                        }
                    data_calibration(100, CH1Data);
                    data_calibration(100, CH2Data);
                    data_calibration(100, CH3Data);
                    //qDebug()<<Count_1000;

                    if (Count_10000 < 10000)
                    {
                        for (int chan = 0; chan < USB->m_LenofChannels; chan++)
                        {
                            CH1DataAdd[chan] = CH1DataAdd[chan] + CH1Data[chan]/10000.0;
                            CH2DataAdd[chan] = CH2DataAdd[chan] + CH2Data[chan]/10000.0;
                            CH3DataAdd[chan] = CH3DataAdd[chan] + CH3Data[chan]/10000.0;
                            //qDebug()<<"CH3Data[" << chan << "] = "<< CH3Data[chan];
                        }
                    }
                    Count_10000++;
                    if (Count_10000 == 10000)
                    {
                        peakNum[0] = findPeaks(CH1DataAdd, m_peakPos[0], m_peakValue[0], m_startPoint, m_ch1TreadHold, m_FBGInterval);
                        peakNum[1] = findPeaks(CH2DataAdd, m_peakPos[1], m_peakValue[1], m_startPoint, m_ch2TreadHold, m_FBGInterval);
                        peakNum[2] = findPeaks(CH3DataAdd, m_peakPos[2], m_peakValue[2], m_startPoint, m_ch3TreadHold, m_FBGInterval);
                        Count_10000 = 10001;
                        //qDebug()<<"PeakNum = "<<peakNum[0];


//                        for(int x = 0; x < 30; x++)
//                        {
//                            qDebug()<<"CH1Peak"<< x << " = "<<m_peakPos[0][x];
//                            qDebug()<<"CH2Peak"<< x << " = "<<m_peakPos[1][x];
//                            qDebug()<<"CH3Peak"<< x << " = "<<m_peakPos[2][x];
//                        }
//                        break;
                    }

                }
            }
        }

        SavePeak2Bin((char*)"C:/DAS/peak1.bin", peakNum[0], m_peakPos[0], m_peakValue[0], CH1DataAdd);

        SavePeak2Bin((char*)"C:/DAS/peak2.bin", peakNum[1], m_peakPos[1], m_peakValue[1], CH2DataAdd);

        SavePeak2Bin((char*)"C:/DAS/peak3.bin", peakNum[2], m_peakPos[2], m_peakValue[2], CH3DataAdd);

        savePeak2Txt((char*)"C:/DAS/peak1.txt", peakNum[0], m_peakPos[0]);

        savePeak2Txt((char*)"C:/DAS/peak2.txt", peakNum[1], m_peakPos[1]);

        savePeak2Txt((char*)"C:/DAS/peak3.txt", peakNum[2], m_peakPos[2]);

    }
    PostMessage(m_hWnd,SEARCH_PEAK_FINISHED,0,0);
    USB->USBClose();
}

void PeakSearch::closeDevice(){

    //USB->USBClose();
    Series->Serial->clear();
    Series->Serial->close();
}

