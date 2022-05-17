#include "demodulation.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <sys/time.h>
#include <QMutex>
#include "CirQueue1.h"




int t1=0,t2=0;
using namespace std;

Demodulation::Demodulation(HWND hWnd, bool setShow):
    SENDSIZE(9000),
    is_SendShowData(setShow)
{
    this->m_hWnd = hWnd;
    sendQueue.setMaxLen(65536000);


}


Demodulation::Demodulation(HWND hWnd,Config *cfig):
    is_SendShowData(false)
{
    this->m_hWnd = hWnd;
    sendQueue.setMaxLen(65536000);

}

Demodulation::Demodulation(bool setSHow, const shared_ptr<GetConfig> gcfg, shared_ptr<CirQueue<float>> CHque, shared_ptr<CirQueue<float>> DMque, shared_ptr<CirQueue<float>> WaveDataque):
    is_SendShowData(setSHow),
    DMgcfg(gcfg),
    CHDataque(CHque),
    DMDataque(DMque),
    DisplayQueue(WaveDataque),
    calcPhase(gcfg->getConfig_calcPhase()),
    isFilter(gcfg->getConfig_Filter()),
    Unwraping(gcfg->getConfig_unWrap()),
    isLPFilter(gcfg->getConfig_LpFilter()),
    Mode(gcfg->getConfig_Debug()),
    peakNum(gcfg->getPeakNum()),
    frequency(gcfg->getConfig_freqency()),
    demoduStop(false),
    soundSave(false),
    startSaveSound(false),
    is_demoduRun(true),
    SENDSIZE(frequency / 100),
    OnceDemoduNum(frequency / 100)//单次解调线程循环的处理数据长度
{
    //DMDataque = make_shared<CirQueue<float>>(CHDataque->MaxLength());//原始数据队列最大容量和解调后数据容量相同
    Vi = new float[peakNum]();
    Vq = new float[peakNum]();
    Ph = new float[peakNum]();
    atanTable = new float[NUMTABLE]();
    readAtanTable(atanTable);

    RealPh = new float[peakNum]();
    PriorPh = new float[peakNum]();
    K=new float[peakNum]();
    PriorK=new float[peakNum]();
    hpcutoff = DMgcfg->getConfig_hpCutOff();
    lpcutoff = DMgcfg->getConfig_lpCutOff();
    FirstRealPh=new float[peakNum]();
    xlast = new float[peakNum][2]();
    mlast = new float[peakNum][2]();
    mlast2 = new float[peakNum][2]();
    mlast3 = new float[peakNum][2]();


    RealPhReg = new float[peakNum][FILTERODR]();
    RealPhOut = new float[peakNum][FILTERODR]();
    FilterReg = new float[peakNum][LPFILTERODR]();
    cnt=new unsigned long[peakNum]();//buttorworth函数执行次数
    FirstIn_n=new int[peakNum]();
    FilterCoeff = new float[54]();
    LPFilterCoeff =new float[10]();
    ReadFilterCoeff(FilterCoeff,LPFilterCoeff, hpcutoff, lpcutoff);
    output = new float[peakNum]();


}


Demodulation::~Demodulation()
{
    FreeMemory();
    qDebug() << "Demodulation deconstruct success" << endl;
}

int& Demodulation::setSENDSIZE(int& sendsize)
{
    SENDSIZE = sendsize;
    return SENDSIZE;
}

void Demodulation::showDemoduImformation(const Config *config)
{
    QString info2 = QString("采样率： ") + QString::number(frequency)
            + QString(" peaknum: ") + QString::number(peakNum);
    emit sendShowQString(info2);

    double norspeed = (double)(frequency * peakNum * 3 * 2 * 8) / 1000000;//Mbps

    QString normalSpeed = QString("Normal Speed is ") + QString::number(norspeed, 'g', 6) + QString("Mbps");
    emit sendShowQString(normalSpeed);

    QString info3;
    if(isFilter == false)
        info3 = QString(" 是否开启滤波： 否 ") + QString("数据存储路径： ") + QString::fromStdString(config->m_DataProcess->m_path );
    else
        info3 = QString(" 是否开启滤波： 是 ") + QString("数据存储路径： ") + QString::fromStdString(config->m_DataProcess->m_path );

    emit sendShowQString(info3);
}

void Demodulation::Init(const Config *cfig, int mode)
{
    if(cfig->m_demodulation->m_CacPhase == std::string("true"))

        this->calcPhase = true;
    else if(cfig->m_demodulation->m_CacPhase == std::string("false"))
        this->calcPhase = false;
    else{}


    if(cfig->m_demodulation->m_Filt == std::string("true"))
        this->isFilter = true;
    else if(cfig->m_demodulation->m_Filt == std::string("false"))
        this->isFilter = false;
    else{}


    if(cfig->m_demodulation->m_Unwrap == std::string("true"))
        this->Unwraping = true;
    else if(cfig->m_demodulation->m_Unwrap == std::string("false"))
        this->Unwraping = false;
    else{}

    if(cfig->m_demodulation->m_LpFilter == std::string("true"))
        this->isLPFilter = true;
    else if(cfig->m_demodulation->m_LpFilter == std::string("false"))
        this->isLPFilter = false;
    else{}


    Mode = mode;
    if(Mode)
    {
        peakNum = cfig->m_Program->m_peaknum;
    }
    else
    {
        peakNum = cfig->m_demodulation->m_peakNum;
    }

    frequency = cfig->m_FPGACard->m_freq;
    demoduStop = false;
    soundSave = false;

    Vi = new float[peakNum]();
    Vq = new float[peakNum]();
    Ph = new float[peakNum]();
    atanTable = new float[NUMTABLE]();
    readAtanTable(atanTable);

    RealPh = new float[peakNum]();
    PriorPh = new float[peakNum]();
    K=new float[peakNum]();
    PriorK=new float[peakNum]();
    hpcutoff = cfig->m_Filter->m_hpcutoff;
    lpcutoff = cfig->m_Filter->m_lpcutoff;
    FirstRealPh=new float[peakNum]();
    xlast = new float[peakNum][2]();
    mlast = new float[peakNum][2]();
    mlast2 = new float[peakNum][2]();
    mlast3 = new float[peakNum][2]();


    RealPhReg = new float[peakNum][FILTERODR]();
    RealPhOut = new float[peakNum][FILTERODR]();
    FilterReg = new float[peakNum][LPFILTERODR]();
    cnt=new unsigned long[peakNum]();//buttorworth函数执行次数
    FirstIn_n=new int[peakNum]();
    FilterCoeff = new float[54]();
    LPFilterCoeff =new float[10]();
    ReadFilterCoeff(FilterCoeff,LPFilterCoeff, hpcutoff, lpcutoff);

    if(!calcPhase)
        demoduType = SEND_ORIGNAL_DATA;
    else if(Unwraping)
    {
        if(isFilter)
        {
            demoduType = SEND_FILTED_PHASE_DATA;
        }
        else
        {
            demoduType = SEND_UNFILT_PHASE_DATA;
        }

    }
    else
    {
        demoduType = SEND_NOT_UNWRAP_PHASE_DATA;
    }

    output = new float[peakNum]();
    //CHDataVc = make_shared<vector<float>>();


    if(mode)
    {
        isUSBOpen = false;

        DebugDemodu = true;



        QString info = QString("Demodulation debugInit done!!");
        emit sendShowQString(info);


        QString info1 = QString("Start DebugDemodu");
        emit sendShowQString(info1);
    }
    else
    {
        this->USB = new USBCtrl(cfig);
        USB->setLenOfChannel(peakNum);
        //USB->setpackagePerRead(10);
        long LEN = USB->packagePerRead * USB->m_LenofChannels * 3 * USB->m_bitCount / 8;
        RECORD_BUF = new BYTE[LEN]();

        qDebug()<<"RECORD_BUF Length = "<< LEN;
        DebugDemodu = false;
        isUSBOpen = false;

    }
    showDemoduImformation(cfig);
    DemoduNum = 0;
//初始化时钟，用于定时发送解调数据给mainwidget显示,和显示的刷新频率匹配。
    timer2 = new QTimer();
    timer2->setInterval(1000);//1s定时
    timer2->start();

    connect(timer2, SIGNAL(timeout()), this, SLOT(timerSlot2()));
}


void Demodulation::readAtanTable(float *roundNum){//读取反正切值查表文件
    QString tableFileName;
    tableFileName = QString("atanTable.bin");
    QString path=QString("C:/DAS/")+tableFileName;//文件地址当在仪表时需要重新设置。
    qDebug()<<"atan table file path:"<<path.toStdString().c_str();
    FILE *tableFile=fopen(path.toStdString().c_str(),"rb");
    if(tableFile == NULL)
    {
        char logStr2[64];
        sprintf(logStr2,"Open %s Failed!\n",path.toStdString().c_str());
        LOG(logStr2,"C:/DAS/log.txt");

    }
    //QMessageBox::information(NULL,"atanTable",path.toStdString().c_str());
    fread(roundNum,sizeof(float),201,tableFile);
    fclose(tableFile);

}

float Demodulation::demoduPh(float vi,float vq){//查表法反正切相位解调
    float absVi=abs(vi);
    float absVq=abs(vq);
    float z=0,z0=0,dz=0,ph0=0,ph=0;
    int numZ0=0;


    if(absVi>=absVq){
        if(vi>0){//111 110
                z=-vq/vi;
                z0=(float) (round(z*100.0)/100.0);//0.01步进的反正切值
                numZ0=(int) (100+z0/0.01);//z0=-1:0.01:1,将其编号为1：1：201
                dz=(z-z0)/(1+(z*z0));
                ph0=dz+atanTable[numZ0];//查表法
                ph=ph0;
        }
        else if(vi==0){
                ph=0;
        }
        else {//vi<0  101 100
            z=-vq/vi;
            z0=(float) (round(z*100.0)/100.0);//0.01步进的反正切值
            numZ0=(int) (100+z0/0.01);//z0=-1:0.01:1,将其编号为1：1：201
            dz=(z-z0)/(1+(z*z0));
            ph0=dz+atanTable[numZ0];//查表法
            ph= ( ((-vq)>=0) ? 1: -1 ) *M_PI+ph0;
        }
    }
    else {//absVi<absVq
            if(vq<0){//-vq>0
                z=-vi/vq;
                z0=(float) (round(z*100.0)/100.0);//0.01步进的反正切值
                numZ0=(int) (100+z0/0.01);//z0=-1:0.01:1,将其编号为1：1：201
                dz=(z-z0)/(1+(z*z0));
                ph0=dz+atanTable[numZ0];//查表法
                ph=0.5*M_PI-ph0;
            }
            else {//-vq<0
                z=-vi/vq;
                z0=(float) (round(z*100.0)/100.0);//0.01步进的反正切值
                numZ0=(int) (100+z0/0.01);//z0=-1:0.01:1,将其编号为1：1：201
                dz=(z-z0)/(1+(z*z0));
                ph0=dz+atanTable[numZ0];//查表法
                ph=-0.5*M_PI-ph0;
            }
    }
    return ph;
}


float Demodulation::IIR_DR(float x, float* plast, float* A, float* B)
{
    float tmp, last;

    tmp = x * B[0];//[0][0]

    //last = tmp - (A[1][1] * plast[0] + A[1][2] * plast[1]);
    last = tmp - (A[4] * plast[0] + A[5] * plast[1]);
//    tmp = last + (B[1][1] * plast[0] + B[1][2] * plast[1]);
    tmp = last + (B[4] * plast[0] + B[5] * plast[1]);

    plast[1] = plast[0];
    plast[0] = last;

    return tmp;
}

float Demodulation::ChebysheyII_HP_Filter(float oriph, int num)//滤波存在问题，暂时不用
{
    //二阶滤波器组合成更高阶数的滤波器
    //qDebug() << "oriph = " << oriph;
    float mid, mid2, mid3, y;

    mid = IIR_DR(oriph, &xlast[num][0], FilterCoeff + 27, FilterCoeff + 0);
    //qDebug() << "mid = " << mid;
    mid2 = IIR_DR(mid, &mlast[num][0], FilterCoeff + 27 + 6, FilterCoeff + 6);
    //qDebug() << "mid2 = " << mid2;
    mid3 = IIR_DR(mid2, &mlast2[num][0], FilterCoeff + 27 + 12, FilterCoeff + 12);
    //qDebug() << "mid3 = " << mid3;
    y = IIR_DR(mid3, &mlast3[num][0], FilterCoeff + 27 + 18, FilterCoeff + 18);
    //更多阶数...
    //qDebug() << "y = " << y << endl;
    float res = y;
    return res;
}

void Demodulation::Hpfilter(int i)
{
    RealPhReg[i][0]=RealPhReg[i][1];
    RealPhReg[i][1]=RealPhReg[i][2];
    RealPhReg[i][2] = RealPh[i]-FirstRealPh[i];

    cnt[i] ++;//滤波执行计数
    if(cnt[i]>2){
    //二阶巴特沃斯滤波，1 * outdata(i)=FilterCoeff(0) * data(i) + FilterCoeff(1) * data(i - 1) + FilterCoeff(2) * data(i-2) - FilterCoeff*outdata(i-1)-b2*outdata(i-2);
    RealPhOut[i][0]=RealPhOut[i][1];
    RealPhOut[i][1]=RealPhOut[i][2];
    //此处的程序已经改成和上面滤波的符号相同，因此以后生成的滤波器系数文件 只需要直接粘贴matlab里fdatools生成的。
    RealPhOut[i][2] = FilterCoeff[0] * RealPhReg[i][2] + FilterCoeff[1] * RealPhReg[i][1] + FilterCoeff[2] * RealPhReg[i][0] - FilterCoeff[3] * RealPhOut[i][1] - FilterCoeff[4] * RealPhOut[i][0];
    cnt[i] = 3;

    }

}

void Demodulation::demoduPhase()
{
    if(soundSave) startSaveSound = true;
    else startSaveSound = false;
    for(int i = 0; i < peakNum; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            float tmp = CHDataque->front();
            //qDebug() << "peaknum = " << i + 1 << " CHnum = " << j + 1 << "data = " << tmp << endl;
            CHDataVc.push_back(tmp);
            if(!calcPhase) DMDataque->push(CHDataque->front());
            CHDataque->pop();
        }
        Vi[i] = CHDataVc[3 * i] + CHDataVc[3 * i + 1] - 2 * CHDataVc[3 * i + 2];
        Vq[i] = -sqrt(3.0000) * (CHDataVc[3 * i] - CHDataVc[3 * i + 1]);

        /*********相位解调*********/
        //Ph[i] = atan2(Vi[i],Vq[i]);
        Ph[i]=demoduPh(Vi[i],Vq[i]);
        float tmp = unWrap_Filter(Ph[i], i);//解缠绕和滤波
        if(calcPhase) DMDataque->push(tmp);
    }
    CHDataVc.clear();

}

void Demodulation::stopDemodulation()
{
    QMutexLocker locker(&m_lock);
    is_demoduRun = false;
}

float Demodulation::unWrap_Filter(const float &ph, const int &i)
{

    if(Unwraping)
    {
        /* 滤波处理
         * */
        if((ph - PriorPh[i]) > M_PI)//PriorPh[i]时间轴上上一个数据
            K[i] = PriorK[i] - 1;//PriorK是时间轴上上一个数据
        else if((ph - PriorPh [i]) < -M_PI)
            K[i] = PriorK[i] + 1;
        else
            K[i] = PriorK[i];

        RealPh[i] = (float)(ph + 2*K[i]*M_PI);
        PriorPh[i] = ph;
        PriorK[i] = K[i];
        if(FirstIn_n[i] == 0)
        {
            FirstRealPh[i] = RealPh[i];
            FirstIn_n[i] = 1;
        }

        RealPh[i] -= FirstRealPh[i];
        //调用高通滤波器函数
        //float res = ChebysheyII_HP_Filter(RealPh[i], i);
        if(isWaveFiletr)
        {
            Hpfilter(i);

        }
        else
        {
            RealPhOut[i][2] = RealPh[i];
        }
        float res = RealPhOut[i][2];

        if(isWaveLpFilter)
        {
            FilterReg[i][0]=LPFilterCoeff[0]*(res-LPFilterCoeff[1]*FilterReg[i][1]-LPFilterCoeff[2]*FilterReg[i][2]-LPFilterCoeff[3]*FilterReg[i][3]-LPFilterCoeff[4]*FilterReg[i][4]);
            output[i] = LPFilterCoeff[5]*FilterReg[i][0]+LPFilterCoeff[6]*FilterReg[i][1]+LPFilterCoeff[7]*FilterReg[i][2]+LPFilterCoeff[8]*FilterReg[i][3]+LPFilterCoeff[9]*FilterReg[i][4];
            for(int n=4;n>=1;n--)
            {
                FilterReg[i][n]=FilterReg[i][n-1];
            }
        }
        else
        {
            output[i] = res;
        }

        if(!(DisplayQueue->isFull())) DisplayQueue->push(output[i]);//显示队列填入带通滤波后数据
        else
        {
            emit sendShowQString(QString("DisplayQueue is full, clear all exist data"));
            DisplayQueue->clear();
        }
        /* 滤波结束开始判断存储队列中存入数据
         * */
        if(startSaveSound) vSound.push_back(output[i]);
        if(isFilter && isLPFilter) return output[i];//带通滤波后数据
        else if(isFilter) return res;//高通滤波后数据
        else return RealPh[i];//无滤波数据

    }
    else
    {
        return ph;//返回非解缠绕数据
    }
}

void Demodulation::debugdemoduPhase(int vectornum, vector<float>& savevec)
{
    int flag=0;
    if(soundSave)
    {
        flag=1;
    }
    else{
        flag=0;
    }


        for(int i = 0; i < peakNum; i++)
        {
            Vi[i] = debugCH1Data[i + vectornum] + debugCH2Data[i + vectornum] - 2 * debugCH3Data[i + vectornum];
            Vq[i] = -sqrt(3.0000) * (debugCH1Data[i + vectornum] - debugCH2Data[i + vectornum]);

                /*********相位解调*********/

            //Ph[i] = atan2(Vi[i],Vq[i]);
            Ph[i]=demoduPh(Vi[i],Vq[i]);
            float tmp = unWrap_Filter(Ph[i], i);//解缠绕和滤波
            savevec.push_back(tmp);
        }

}


vector<float> Demodulation::debugReadData(QString filename, bool &filend, int &startpos)
{
    QString path = QString("F:/mystudy/2020yan/shui_ting/data/") + filename;
    string stdpath = path.toStdString();
    vector<float> orginVec;

    QFile pf(path);
    /************read**********/
    if (pf.exists() == false) {
        QString info = path + QString(" dont exist");
        emit sendShowQString(info);

    } else
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
                orginVec.push_back(orginData);
            
        }
        if(!filend) startpos = inbin.tellg();//获取当前已经读取的文件位置
        else
            filend = false;

        inbin.close();

    }

    return orginVec;
    //orginVec.clear();
}


void Demodulation::debugRunDemodu()
{

    qDebug()<<"Demodulation Current Thread : "<<QThread::currentThread();
    QString info = QString("debugRunDemodu start");
    emit sendShowQString(info);


    int t1=0,t2=0;
    int term = 0;
    int hasRecv=0;

    QString CH1filename = QString("[CH1][10]20220112203715.bin");
    bool CH1filend = false;
    int CH1startpos = 0;

    QString CH2filename = QString("[CH2][10]20220112203715.bin");
    bool CH2filend = false;
    int CH2startpos = 0;

    QString CH3filename = QString("[CH3][10]20220112203715.bin");
    bool CH3filend = false;
    int CH3startpos = 0;
    is_demoduRun = true;

    while(true)
    {
        if (!is_demoduRun) return;

        hasRecv = 300;
        //qDebug()<<"hasRecv"<<hasRecv<<endl;
        CHdatalen+=hasRecv;
        if(CHdatalen>=10000000){
            CHdatalen=0;
        }


        if (hasRecv > 0)
        {
            int t3 = 0;
            t3 = int(clock());
             debugCH1Data = debugReadData(CH1filename, CH1filend, CH1startpos);
             debugCH2Data = debugReadData(CH2filename, CH2filend, CH2startpos);
             debugCH3Data = debugReadData(CH3filename, CH3filend, CH3startpos);
             //qDebug() << "CH1startpos" << CH1startpos;
             if(CH1filend || CH2filend || CH3filend)
             {
                 QString info = QString("CHfilend");
                 emit sendShowQString(info);
                 CH1startpos = CH2startpos = CH3startpos = 0;
                 CH1filend = CH2filend = CH1filend = false;
             }
            while( (int(clock()) - t3) < 10 );


             for (int j = 0; j < hasRecv; j++)
             {

                  t2=int(clock())-t1;

                  if(t2>=10000)
                  {

                      qDebug()<<"CHdatalen"<<CHdatalen;
                      qDebug()<<"t1:"<<t1<<"t2:"<<t2;
                      //fwrite(CH1DataBuff[0],sizeof(unsigned short),CHdatalen[0],pFile[0]);
                      t1=int(clock());
                  }
                    vector<float> tmpvec;
                    debugdemoduPhase(j * peakNum, tmpvec);
                    if(calcPhase)
                    {
                        for(auto tmp : tmpvec)
                        {
                            sendQueue.push(tmp);
                        }
                    }
                    else
                    {
                        for(int nn = 0; nn < peakNum ; nn++)
                        {
                            sendQueue.push(debugCH1Data[nn + j * peakNum]);
                            sendQueue.push(debugCH2Data[nn + j * peakNum]);
                            sendQueue.push(debugCH3Data[nn + j * peakNum]);
                        }
                    }
                    term++;
                    DemoduNum++;

                    if(term == SENDSIZE)
                    {
                        //qDebug()<<"SEND";
                        //emit sendData(SENDSIZE);
                        //qDebug() << "sendWaveData len = " << DisplayQueue.size() << endl;
                        //if(is_SendShowData) emit sendWaveData(&DisplayQueue, peakNum);
                        emit sendDataBegin(&sendQueue,demoduType);
                        term = 0;
                        //qDebug() << &sendQueue << endl;
                    }



            }
             debugCH1Data.clear();
             debugCH2Data.clear();
             debugCH3Data.clear();

        }

    }

    qDebug() << "stop demodulation" << endl;
}


void Demodulation::timerSlot2()
{
    if(sender() == timer2)
    {
        double Speed = (double)(DemoduNum * peakNum * 3 * 2 * 8) / 1000000;//Mbps
        DemoduNum = 0;
        emit sendSpeed(Speed);
        //qDebug() << Speed << "Mbps" << endl;
    }
}

void Demodulation::getCHdata(int &i, int &j, int &num)
{
    for (i = j * peakNum * USB->m_ChannelCount * 2;
         i < peakNum * 2 + j * USB->m_ChannelCount * peakNum * 2;
         i += 2)
    {
         num = (i - j * USB->m_ChannelCount * peakNum * 2) / 2;


     }
    //qDebug() << "num = " <<  num << endl;
}

void Demodulation::run()
{
    /* 在信息窗口中显示相关配置信息
     */
    QString Typeinfo;
    if(!isWaveFiletr)
        Typeinfo += QString("HpFilter File don't exist");
    if(!isWaveLpFilter)
        Typeinfo += QString("LpFilter File don't exist");
    emit sendShowQString(Typeinfo);
    if(!calcPhase)
    {
        demoduType = SEND_ORIGNAL_DATA;
        Typeinfo = QString("存储原始三通道数据...");
    }
    else if(Unwraping)
    {
        if(isFilter)
        {
            demoduType = SEND_FILTED_PHASE_DATA;
            Typeinfo = QString("存储解调后滤波数据...");
        }
        else
        {
            demoduType = SEND_UNFILT_PHASE_DATA;
            Typeinfo = QString("存储解调后无滤波数据...");
        }
    }
    else
    {
        demoduType = SEND_NOT_UNWRAP_PHASE_DATA;
        Typeinfo = QString("存储未解缠绕数据...");
    }
    emit sendShowQString(Typeinfo);

    if(Mode)
    {
        QString info = QString("Demodulation debugInit done!!");
        emit sendShowQString(info);
        QString info1 = QString("Start DebugDemodu");
        emit sendShowQString(info1);
    }
    else
    {
        QString info = QString("Demodulation Init done!!");
        emit sendShowQString(info);
        QString info1 = QString("Start Demodu");
        emit sendShowQString(info1);
    }
    showDemoduImformation(DMgcfg->getConstPointoConfig());//不能放在构造函数中，放在构造函数中发出的signal中携带的信息失效
    qDebug() << "start Demodulation";
    QString info = QString("RunDemodu start");
    emit sendShowQString(info);
    /* 开启解调线程
     */
    while(1)
    {
        if(!is_demoduRun) return;//外界关闭解调循环
        if(CHDataque->size() < 3 * peakNum * OnceDemoduNum)//单次解调线程循环的最少解调数据量为0.1s数据
        {
            //qDebug() << "waiting...";
            msleep(50);//原始数据队列中数据过少，线程等待0.05s
        }
        else
        {
            int num = 0;
            while(++num <= OnceDemoduNum) demoduPhase();
            //qDebug() << "DMque size = " << DMDataque->size() << endl;
        }



    }
}

void Demodulation::ReadFilterCoeff(float *coeff,float *LPFcoeff, string hpcutoff, string lpcutoff)//读取文件需要重写用ifstrem
{
    QString fileName = QString::fromLocal8Bit(hpcutoff.data());
    QString fileName2 = QString::fromLocal8Bit(lpcutoff.data());

    //fileName = QString("ButtorWorthFilterCoefficient_")+QString::number((int)(frequency/1000))+QString("KHz_50Hz.bin");
    //fileName = QString("ChebysheyIIFilterCoefficient_Order8_4Section_")+QString::number((int)(frequency/1000))+QString("KHz_1Hz.bin");//1Hz的高通滤波器文件名为ButtorWorthFilterCoefficient_Order8_1KHz_1Hz
    //fileName2 = QString("LPFilterCoefficient_")+QString::number((int)(frequency/1000))+QString("KHz_400Hz.bin");// 400Hz低通滤波器文件名LPFilterCoefficient_1KHz_400Hz.bin

    QString pat = QString("C:/DAS/ButtorWorthFilterCoefficient/") + fileName;
    QString pat2 = QString("C:/DAS/ButtorWorthFilterCoefficient/") + fileName2;

    qDebug()<<"Filter Coefficient path:"<<pat.toStdString().c_str()<<"  "<<pat2.toStdString().c_str();

    QFile pFile(pat);
    QFile pFile2(pat2);
    if(!pFile.exists())
    {
        char logStr[64];
        sprintf(logStr,"Open %s Failed!.",pat.toStdString().c_str());
        LOG(logStr,"C:/DAS/log.txt");
//        QString info = QString("HpFilter file open open failed :") + pat;
//        emit sendShowQString(info);
        isWaveFiletr = false;
    }
    else
    {
        isWaveFiletr = true;
        ifstream inFile(pat.toStdString().data(), ifstream::binary);
        for(int i = 0 ; i < FILTERODR; i++)
        {
            inFile.read((char*)(coeff + i), sizeof(float));
            qDebug() << "coeff[" << i << "] = " << coeff[i] << endl;
        }
        inFile.close();
    }

    if(!pFile2.exists())
    {
        char logStr[64];
        sprintf(logStr,"Open %s Failed!.",pat2.toStdString().c_str());
        LOG(logStr,"C:/DAS/log.txt");
//        QString info = QString("LpFilter file open open failed :") + pat2;
//        emit sendShowQString(info);
        isWaveLpFilter = false;
    }
    else
    {
        isWaveLpFilter = true;
        ifstream inFile2(pat2.toStdString().data(), ifstream::binary);
        for(int i = 0 ; i < LPFILTERODR; i++)
        {
            inFile2.read((char*)(LPFcoeff + i), sizeof(float));
            qDebug() << "LPFcoeff[" << i << "] = " << LPFcoeff[i] << endl;
        }
        inFile2.close();
    }

}


/******************该函数有bug******************/
//void Demodulation::FilterCoeffCalculate(float *coeff){//fre=5,res=1.5,FilterOrder=5,calculate buttorworthFilte coeff
//    float res=1.5;
//    float c=tan(M_PI*fre/frequency);
//    coeff[0]=1/(1+res*c+pow(c,2));
//    coeff[1]=(-2)*coeff[0];
//    coeff[2]=coeff[0];
//    coeff[3]=2*(pow(c,2)-1)*coeff[0];
//    coeff[4]=(1-res*c+pow(c,2))*coeff[0];
//}

void Demodulation::FreeMemory()
{
    delete[] output;
    delete[] Vi;
    delete[] Vq;
    delete[] Ph;
    delete[] atanTable;


    delete[] RealPh;
    delete[] PriorPh;
    delete[] K;
    delete[] PriorK;
    delete[] FilterCoeff;
    delete[] xlast;
    delete[] mlast;
    delete[] mlast2;
    delete[] mlast3;
    delete[] LPFilterCoeff;
    delete[] FirstRealPh;
    delete[] FirstIn_n;
    delete[] RealPhReg;
    delete[] RealPhOut;
    delete[] FilterReg;
    delete[] cnt;
}

bool Demodulation::openDevice()
{
    bool isopen = false;
    if(USB->USBStart() == -1)
        isopen = true;

    return isopen;
}
