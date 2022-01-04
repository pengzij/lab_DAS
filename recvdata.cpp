#include "recvdata.h"

RecvData::RecvData(const shared_ptr<GetConfig> gcfg, shared_ptr<CirQueue<float>> que):
    CHdata(que),
    RDgcfg(gcfg),
    peakNum(gcfg->getPeakNum()),
    isDebug(gcfg->getConfig_Debug()),
    is_RecvRun(true)
{
    if(isDebug)
    {
        debugCH1Data = make_shared<vector<float>>();
        debugCH2Data = make_shared<vector<float>>();
        debugCH3Data = make_shared<vector<float>>();
    }
    else
    {
        USB = make_shared<USBCtrl>(gcfg->getConstPointoConfig());
        USB->setLenOfChannel(peakNum);
        long LEN = RDgcfg->getConfig_packagePerRead() * RDgcfg->getConfig_channelLength() * RDgcfg->getConfig_bitCount() / 8;
        RECORD_BUF = make_shared<BYTE*>(new BYTE[LEN]());
        qDebug() << "RECORD_BUF Length = " << LEN;
    }
    /* 初始化时钟，用于定时发送解调数据给mainwidget显示,和显示的刷新频率匹配。
    */
    timer = make_shared<QTimer>();//1s定时
    timer->setInterval(1000);
    timer->start();
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(timerSlot()) );
}

RecvData::~RecvData()
{
    if(!isDebug)
    {
        USB->USBClose();
    }
    qDebug() << "RecvData deconstruct success" << endl;
}

void RecvData::run()
{
    qDebug() << "start recvData";

    if(isDebug)
    {
        debugRecvData();
    }
    else
    {
        RecvDataRun();
    }


}

void RecvData::debugRecvData()
{
    int hasRecv=0;
    QString CH1filename = QString("[CH1][30]20211008163701.bin");
    bool CH1filend = false;
    int CH1startpos = 0;

    QString CH2filename = QString("[CH2][30]20211008163701.bin");
    bool CH2filend = false;
    int CH2startpos = 0;

    QString CH3filename = QString("[CH3][30]20211008163701.bin");
    bool CH3filend = false;
    int CH3startpos = 0;
    while(1)
    {
        qDebug() << "CHDATA size" << CHdata->size() << endl;
        if(!is_RecvRun) return;//关闭读取数据死循环
//        if(CHdata->size() > 900 * 100 ^3)
//        {
//            qDebug() << "CHDATA size" << CHdata->size() << endl;
//            return;//如果有30分钟的数据拥挤退出
//        }

        hasRecv = 300;
        int t3 = 0;
        t3 = int(clock());
        debugReadData(CH3filename, CH3filend, CH3startpos, debugCH3Data);
        debugReadData(CH1filename, CH1filend, CH1startpos, debugCH1Data);
        debugReadData(CH2filename, CH2filend, CH2startpos, debugCH2Data);

        if(CH1filend || CH2filend || CH3filend)
        {
            qDebug() << "CHfileEnd";
            CH1startpos = CH2startpos = CH3startpos = 0;
            CH1filend = CH2filend = CH1filend = false;
        }
        while( (int(clock()) - t3) < 10 ){}//限制采样率不超过30kHz
        for(int RecvNo = 0; RecvNo < hasRecv; ++RecvNo)
        {
            for(int peakNo = 0; peakNo < peakNum; ++peakNo)
            {

                CHdata->push(debugCH1Data->operator [](peakNo + RecvNo * peakNum));
                CHdata->push(debugCH2Data->operator [](peakNo + RecvNo * peakNum));
                CHdata->push(debugCH3Data->operator [](peakNo + RecvNo * peakNum));
            }
            ++hasRecvNum;

        }
        debugCH1Data->clear();
        debugCH2Data->clear();
        debugCH3Data->clear();
        qDebug() << "recv run end" << endl;
    }
}

void RecvData::RecvDataRun()
{
    int usb = USB->USBStart();
    USB->USBClose();
    usb = USB->USBStart();
    qDebug()<<"USB Return = "<< usb;
    int hasRecv = 0;
    while(1)
    {
        if(!is_RecvRun) return;//关闭读取数据死循环
        hasRecv = USB->getDataFromUSB(*RECORD_BUF);
        if(hasRecv > 0)
        {
            int num = 0;
            int i =0;
            for(int j = 0; j < hasRecv; j++)
            {
                getDatafromByteTofloat(i, j, num);
                ++hasRecvNum;
            }
        }
    }
}

void RecvData::stopRecvData()
{
    is_RecvRun = false;
}

void RecvData::getDatafromByteTofloat(int &i, int &j, int &num)
{
    for (i = j * peakNum * USB->m_ChannelCount * 2;
         i < peakNum * 2 + j * USB->m_ChannelCount * peakNum * 2;
         i += 2)
    {
        num = (i - j * USB->m_ChannelCount * peakNum * 2) / 2;//num = 0 ~ peakNum
        for(int k = 0; k < 3; k++)
        {
            unsigned short usCHDATA = (  (*RECORD_BUF)[i + 1 + peakNum * 2 * k] << 8) + ((*RECORD_BUF)[i + peakNum * 2 * k]);
            CHdata->push(float(usCHDATA));
        }
    }
}

void RecvData::debugReadData(QString& filename, bool &filend, int &startpos, shared_ptr<vector<float>>& debugCHdata)
{
    QString path = QString("F:/mystudy/2020yan/shui_ting/data/") + filename;
    string stdpath = path.toStdString();
    QFile pf(path);
    /************read**********/
    if (pf.exists() == false) {
        qDebug() << "CHdata don't exist";

    }
    else
    {

        ifstream inbin(stdpath.data(), std::ifstream::binary);
        inbin.seekg(startpos);// 定位到文件中startpos所在位置开始读取
        for(int i = 0; i < 300 * peakNum; i++)
        {
                float orginData;
                inbin.read((char*)(&orginData), sizeof(float));
                if( inbin.eof() )//读取到文件末尾
                {
                    filend = true;
                    startpos = 0;
                    break;
                }
                debugCHdata->push_back(orginData);

        }
        if(!filend) startpos = inbin.tellg();//获取当前已经读取的文件位置
        else
            filend = false;

        inbin.close();

    }
}

void RecvData::timerSlot()
{
    double Speed = (double)(hasRecvNum * peakNum * 3 * 2 * 8) / 1000000;//Mbps
    hasRecvNum = 0;
    qDebug() << "Speed = " << Speed;
    emit sendSpeed(Speed);
}
