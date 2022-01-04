#include "USBCtrl.h"
#include <QDebug>
#include <QMessageBox>
USBCtrl::USBCtrl(const Config *cfig)
{
	this->setParam(cfig);
}

USBCtrl::USBCtrl(){}

USBCtrl::~USBCtrl(){}


void USBCtrl::setParam(const Config *cfig) {
	m_bitCount = cfig->m_FPGACard->m_bitCount;
	m_Freq = cfig->m_FPGACard->m_freq;
	m_ChannelCount = cfig->m_FPGACard->m_channelCount;
	m_LenofChannels = cfig->m_FPGACard->m_channelLen;
    packagePerRead = cfig->m_FPGACard->m_packagePerRead;
}

void USBCtrl::setParam(shared_ptr<GetConfig>& PSgcfg) {
    m_bitCount = PSgcfg->getConfig_bitCount();
    m_Freq = PSgcfg->getConfig_freqency();
    m_ChannelCount = PSgcfg->getConfig_channelCount();
    m_LenofChannels = PSgcfg->getConfig_channelLength();
    packagePerRead = PSgcfg->getConfig_packagePerRead();
}


void USBCtrl::setParam(int chanCount, int Freq, int bitCount, int lenOfChannal,int pakgPerRead) {
	m_bitCount = bitCount;
	m_ChannelCount = chanCount;
	m_LenofChannels = lenOfChannal;
	m_Freq = Freq;
    this->packagePerRead = pakgPerRead;
}


int USBCtrl::USBStart() {
    qDebug() << "USBStart start USBCtrl 32" << endl;
	bool step1 = false;
	step1 = CU3_Init();
	if (!step1)
	{
		return 1;
	}
    usb_dev = CU3_OpenDev(TEXT("1"));
	if (usb_dev == nullptr)
	{
		return 2;
	}
	bool step3 = false;
	struct CU3_SAM_PARA *cu3_parameter;
	cu3_parameter = (CU3_SAM_PARA*)malloc(sizeof(struct CU3_SAM_PARA));
	cu3_parameter->nBitCount = m_bitCount;
	cu3_parameter->nChannelCount = m_ChannelCount;
	cu3_parameter->nFreq = m_Freq;
	for (int i = 0; i<3; i++)
		cu3_parameter->nRecordLenOfChans[i] = m_LenofChannels;
	step3 = CU3_SetSamParam(usb_dev, cu3_parameter);
	free(cu3_parameter);
	if (!step3)
	{
		return 3;
	}
	bool step4 = false;
	step4 = CU3_StartAD(usb_dev);
	if (!step4)
	{
		return 4;
	}
    qDebug() << "USBStart end USBCtrl 64" << endl;
	return -1;
}


int USBCtrl::USBClose()
{
	bool step6 = false;
	step6 = CU3_StopAD(usb_dev);
	if (!step6)
	{
		return 6;
	}
	bool step7 = false;
	step7 = CU3_CloseDev(usb_dev);
	if (!step7)
	{
		return 7;
	}
	CU3_Release();
	return -1;
}

void USBCtrl::setpackagePerRead(int size) {
	packagePerRead = size;
}

int USBCtrl::getDataFromUSB(BYTE *recvBuf) {
	int hasRecv = 0;
    //qDebug()<<"packagePerRead" << packagePerRead;
	CU3_RECORD_META *CU3_RECORD = new CU3_RECORD_META[packagePerRead];
    memset(CU3_RECORD, 0, sizeof(CU3_RECORD_META)*packagePerRead);//用指针后面的N个字节全部用0替换
    //qDebug() << "ReadADRecords start USBCtrl line 96" << endl;
	hasRecv = CU3_ReadADRecords(usb_dev, recvBuf, packagePerRead, CU3_RECORD);
    //qDebug() << "ReadADRecords end USBCtrl line 98" << endl;
	delete[] CU3_RECORD;
    //qDebug() << "haswrite = " << hasRecv << endl;
	if (hasRecv > 0)
		return hasRecv;
	else
		return 0;
}
