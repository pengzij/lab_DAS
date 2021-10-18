#include "demodulation.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <sys/time.h>
#include <QMutex>
#include "CirQueue1.h"




int t1=0,t2=0;
using namespace std;

Demodulation::Demodulation(HWND hWnd)
{
    this->m_hWnd = hWnd;
    sendQueue.setSize(65536000);
    DisplayQueue.setSize(65536000);

}


Demodulation::Demodulation(HWND hWnd,Config *cfig)
{
    this->m_hWnd = hWnd;
    Init(cfig);
    sendQueue.setSize(65536000);
    DisplayQueue.setSize(65536000);
}

Demodulation::~Demodulation()
{

    FreeMemory();
    USB->USBClose();
    delete USB;
    qDebug() << "Demodulation deconstruct success" << endl;
}

void Demodulation::debugInit(Config *cfig)
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

    peakNum = cfig->m_Program->m_peaknum;
    qDebug() << "peakNu = " <<  peakNum;
    frequency = cfig->m_FPGACard->m_freq;
    demoduStop = false;
    soundSave = false;

//    if(UnwrapFilter){
//        unwrapFilReg = new float[peakNum][3];
//    }

    if(!calcPhase)
        demoduType = SEND_ORIGNAL_DATA;
    else
    {
        Vi = new float[peakNum]();
        Vq = new float[peakNum]();
        Ph = new float[peakNum]();
        atanTable = new float[NUMTABLE]();
        readAtanTable(atanTable);
        if(Unwraping)
        {

            RealPh = new float[peakNum]();
            PriorPh = new float[peakNum]();
            K=new float[peakNum]();
            PriorK=new float[peakNum]();

            if(isFilter){
                demoduType = SEND_FILTED_PHASE_DATA;
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

            }
            else{
                demoduType = SEND_UNFILT_PHASE_DATA;
            }
        }
        else
        {
            demoduType = SEND_NOT_UNWRAP_PHASE_DATA;
        }
    }

    output = new float[peakNum]();

    CH1Data = new unsigned short[peakNum]();
    CH2Data = new unsigned short[peakNum]();
    CH3Data = new unsigned short[peakNum]();

    /*
    this->USB = new USBCtrl(cfig);
    USB->setLenOfChannel(peakNum);
    //USB->setpackagePerRead(10);
    long LEN = USB->packagePerRead * USB->m_LenofChannels * 3 * USB->m_bitCount / 8;
    RECORD_BUF = new BYTE[LEN]();

    qDebug()<<"RECORD_BUF Length = "<<LEN;
    */


    isUSBOpen = false;

    DebugDemodu = true;



    QString info = QString("Demodulation debugInit done!!");
    emit sendShowQString(info);


    QString info1 = QString("Start DebugDemodu");
    emit sendShowQString(info1);

    QString info2 = QString("采样率： ") + QString::number(frequency)
            + QString(" peaknum: ") + QString::number(peakNum);
    emit sendShowQString(info2);
    QString info3;
    if(isFilter == false)
        info3 = QString(" 是否开启滤波： 否 ") + QString("数据存储路径： ") + QString::fromStdString(cfig->m_DataProcess->m_path );
    else
        info3 = QString(" 是否开启滤波： 是 ") + QString("数据存储路径： ") + QString::fromStdString(cfig->m_DataProcess->m_path );

    emit sendShowQString(info3);

}

void Demodulation::Init(Config *cfig)
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

    peakNum = cfig->m_demodulation->m_peakNum;
    frequency = cfig->m_FPGACard->m_freq;
    demoduStop = false;
    soundSave = false;

//    if(UnwrapFilter){
//        unwrapFilReg = new float[peakNum][3];
//    }

    if(!calcPhase)
        demoduType = SEND_ORIGNAL_DATA;
    else
    {
        Vi = new float[peakNum]();
        Vq = new float[peakNum]();
        Ph = new float[peakNum]();
        atanTable = new float[NUMTABLE]();
        readAtanTable(atanTable);
        if(Unwraping)
        {

            RealPh = new float[peakNum]();
            PriorPh = new float[peakNum]();
            K=new float[peakNum]();
            PriorK=new float[peakNum]();

            if(isFilter){
                demoduType = SEND_FILTED_PHASE_DATA;
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

            }
            else{
                demoduType = SEND_UNFILT_PHASE_DATA;
            }
        }
        else
        {
            demoduType = SEND_NOT_UNWRAP_PHASE_DATA;
        }
    }

    output = new float[peakNum]();

    CH1Data = new unsigned short[peakNum]();
    CH2Data = new unsigned short[peakNum]();
    CH3Data = new unsigned short[peakNum]();


    this->USB = new USBCtrl(cfig);
    USB->setLenOfChannel(peakNum);
    //USB->setpackagePerRead(10);
    long LEN = USB->packagePerRead * USB->m_LenofChannels * 3 * USB->m_bitCount / 8;
    RECORD_BUF = new BYTE[LEN]();

    qDebug()<<"RECORD_BUF Length = "<<LEN;


    QString info2 = QString("采样率：") + QString::number(frequency)
            + QString("  peaknum:") + QString::number(peakNum);
    //emit sendShowQString(info2);
    QString info3;
    if(isFilter == false)
        info3 = QString(" 是否开启滤波：否 ") + QString(" 数据存储路径：") + QString::fromStdString(cfig->m_DataProcess->m_path );
    else
        info3 = QString(" 是否开启滤波：是 ") + QString(" 数据存储路径：") + QString::fromStdString(cfig->m_DataProcess->m_path );
    info3 = info2 + info3;
    emit sendShowQString(info3);

    DebugDemodu = false;
    isUSBOpen = false;
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
            Vi[i] = CH1Data[i] + CH2Data[i] - 2 * CH3Data[i];
            Vq[i] = -sqrt(3.0000) * (CH1Data[i] - CH2Data[i]);

                /*********相位解调*********/

            //Ph[i] = atan2(Vi[i],Vq[i]);
            Ph[i]=demoduPh(Vi[i],Vq[i]);
            //Ph[i]=0;
            if(Unwraping)
            {
                if((Ph[i] - PriorPh[i]) > M_PI)//PriorPh[i]时间轴上上一个数据
                    K[i] = PriorK[i] - 1;//PriorK是时间轴上上一个数据
                else if((Ph[i] - PriorPh [i]) < -M_PI)
                    K[i] = PriorK[i] + 1;
                else
                    K[i] = PriorK[i];

                RealPh[i] = (float)(Ph[i] + 2*K[i]*M_PI);
                PriorPh[i] = Ph[i];
                PriorK[i] = K[i];

                if(isFilter)
                {
                    if(FirstIn_n[i]==0){
                        FirstRealPh[i]=RealPh[i];
                        FirstIn_n[i]=1;
                    }
                    RealPh[i] -= FirstRealPh[i];
                    //调用高通滤波器函数
                    //float res = ChebysheyII_HP_Filter(RealPh[i], i);

                    Hpfilter(i);
                    float res = RealPhOut[i][2];

                    if(isLPFilter){
                        FilterReg[i][0]=LPFilterCoeff[0]*(res-LPFilterCoeff[1]*FilterReg[i][1]-LPFilterCoeff[2]*FilterReg[i][2]-LPFilterCoeff[3]*FilterReg[i][3]-LPFilterCoeff[4]*FilterReg[i][4]);
                        output[i] = LPFilterCoeff[5]*FilterReg[i][0]+LPFilterCoeff[6]*FilterReg[i][1]+LPFilterCoeff[7]*FilterReg[i][2]+LPFilterCoeff[8]*FilterReg[i][3]+LPFilterCoeff[9]*FilterReg[i][4];
                        for(int n=4;n>=1;n--){
                            FilterReg[i][n]=FilterReg[i][n-1];
                            }
                    }
                    else{
                         output[i] = res;
                    }
                    if(flag == 1){
                    vSound.push_back(output[i]);
                    sendQueue.push(output[i]);
                    }
                    else{
                    sendQueue.push(output[i]);
                    }

                }
                else
                {
                    output[i] = RealPh[i];
                    sendQueue.push(output[i]);

                }
            }
            else
            {
                output[i] = Ph[i];

                sendQueue.push(output[i]);
            }
        }

}

void Demodulation::stopDemodulation()
{
    QMutexLocker locker(&m_lock);
    is_demoduRun = false;
}

void Demodulation::debugdemoduPhase(int vectornum)
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

            Ph[i]=demoduPh(Vi[i],Vq[i]);
            //Ph[i] = atan(Vi[i] / Vq[i]);
            //Ph[i]=0;
            if(Unwraping)
            {
                if((Ph[i] - PriorPh[i]) > M_PI)//PriorPh[i]时间轴上上一个数据
                    K[i] = PriorK[i] - 1;//PriorK是时间轴上上一个数据
                else if((Ph[i] - PriorPh [i]) < -M_PI)
                    K[i] = PriorK[i] + 1;
                else
                    K[i] = PriorK[i];

                RealPh[i] = (float)(Ph[i] + 2*K[i]*M_PI);
                PriorPh[i] = Ph[i];
                PriorK[i]=K[i];

                if(isFilter)
                {
                    if(FirstIn_n[i]==0){
                        FirstRealPh[i]=RealPh[i];
                        FirstIn_n[i]=1;
                    }
                    RealPh[i] -= FirstRealPh[i];
                    //调用高通滤波器函数
                    //float res = ChebysheyII_HP_Filter(RealPh[i], i);
                    //qDebug() << "res = " << res;


                    Hpfilter(i);
                    float res = RealPhOut[i][2];

                    if(isLPFilter){
                        FilterReg[i][0]=LPFilterCoeff[0]*(res-LPFilterCoeff[1]*FilterReg[i][1]-LPFilterCoeff[2]*FilterReg[i][2]-LPFilterCoeff[3]*FilterReg[i][3]-LPFilterCoeff[4]*FilterReg[i][4]);
                        output[i] = LPFilterCoeff[5]*FilterReg[i][0]+LPFilterCoeff[6]*FilterReg[i][1]+LPFilterCoeff[7]*FilterReg[i][2]+LPFilterCoeff[8]*FilterReg[i][3]+LPFilterCoeff[9]*FilterReg[i][4];
                        for(int n=4;n>=1;n--){
                            FilterReg[i][n]=FilterReg[i][n-1];
                            }
                        //qDebug() << "output = " << output[i];
                    }
                    else{
                         output[i] = res;
                    }


                    if(flag == 1){
                    vSound.push_back(output[i]);
                    sendQueue.push(output[i]);
                    }
                    else{
                    sendQueue.push(output[i]);
                    }

                }
                else
                {
                    output[i] = RealPh[i];
                    sendQueue.push(output[i]);

                }
            }
            else
            {
                output[i] = Ph[i];

                sendQueue.push(output[i]);
            }
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

        for(int i = 0; i < 10 * peakNum; i++)
        {
            if( inbin.eof() )//读取到文件末尾
            {
                filend = true;
                startpos = 0;
                break;
            }
            else
            {
                float orginData;
                inbin.read((char*)(&orginData), sizeof(float));
                orginVec.push_back(orginData);
            }
        }
        startpos = inbin.tellg();//获取当前已经读取的文件位置
        //qDebug() << "startpos = " << startpos;
        if(inbin.eof())
        {
            filend = true;
            startpos = 0;
        }

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
    is_demoduRun = true;

   QString CH1filename = QString("[CH1][30]20211008163701.bin");
    bool CH1filend = false;
    int CH1startpos = 0;

    QString CH2filename = QString("[CH2][30]20211008163701.bin");
    bool CH2filend = false;
    int CH2startpos = 0;

    QString CH3filename = QString("[CH3][30]20211008163701.bin");
    bool CH3filend = false;
    int CH3startpos = 0;

    while(true)
    {

        if (!is_demoduRun) break;

        hasRecv = 10;
        //qDebug()<<"hasRecv"<<hasRecv<<endl;
        CHdatalen+=hasRecv;
        if(CHdatalen>=10000000){
            CHdatalen=0;
        }

        if (hasRecv > 0)
        {

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


                    if(calcPhase)
                    {
                        this->debugdemoduPhase(j * peakNum);
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

                    if(term == SENDSIZE)
                    {
                        qDebug()<<"SEND";
                        //emit sendData(SENDSIZE);
                        emit sendDataBegin(&sendQueue,demoduType);
                        term = 0;
                    }

            }
             debugCH1Data.clear();
             debugCH2Data.clear();
             debugCH3Data.clear();

        }




    }

    qDebug() << "stop demodulation" << endl;
}


void Demodulation::run()
{
    qDebug() << "DebugDemodu = " << DebugDemodu;
    if(DebugDemodu)
    {
        debugRunDemodu();
    }
    else
    {
        qDebug()<<"Demodulation Current Thread : "<<QThread::currentThread();
        int usb = USB->USBStart();
        USB->USBClose();
        usb = USB->USBStart();
        qDebug()<<"USB Return = "<< usb;
        int t1=0,t2=0;
        int term = 0;
        int hasRecv=0;
        is_demoduRun = true;

        while(true)
        {

            if (!is_demoduRun)
            {
                return;
            }

            hasRecv = USB->getDataFromUSB(RECORD_BUF);
            //qDebug()<<"hasRecv"<<hasRecv<<endl;
            CHdatalen+=hasRecv;
            if(CHdatalen>=10000000){
                CHdatalen=0;
            }
            if (hasRecv > 0)
            {
                 int num = 0;
                 int i = 0;
                 for (int j = 0; j < hasRecv; j++)
                 {

                      for (i = j * USB->m_LenofChannels * USB->m_ChannelCount * 2;
                           i < USB->m_LenofChannels * 2 + j * USB->m_ChannelCount * USB->m_LenofChannels * 2;
                           i += 2)
                      {
                           num = (i - j * USB->m_ChannelCount * USB->m_LenofChannels * 2) / 2;

                           CH1Data[num] = RECORD_BUF[i + 1];
                           CH1Data[num] = (CH1Data[num] << 8) + (RECORD_BUF[i]);


                           CH2Data[num] = RECORD_BUF[i + USB->m_LenofChannels * 2 + 1];
                           CH2Data[num] = (CH2Data[num] << 8) + (RECORD_BUF[i + USB->m_LenofChannels * 2]);


                           CH3Data[num] = RECORD_BUF[i + USB->m_LenofChannels * 4 + 1];
                           CH3Data[num] = (CH3Data[num] << 8) + (RECORD_BUF[i + USB->m_LenofChannels * 4]);

                       }
                      t2=int(clock())-t1;

                      if(t2>=10000)
                      {

                          qDebug()<<"CHdatalen"<<CHdatalen;
                          qDebug()<<"t1:"<<t1<<"t2:"<<t2;
                          //fwrite(CH1DataBuff[0],sizeof(unsigned short),CHdatalen[0],pFile[0]);
                          t1=int(clock());
                      }


                        if(calcPhase)
                        {
                            this->demoduPhase();

                        }
                        else
                        {
                            for(int nn = 0; nn < peakNum ; nn++)
                            {
                                sendQueue.push(CH1Data[nn]);
                                sendQueue.push(CH2Data[nn]);
                                sendQueue.push(CH3Data[nn]);
                            }
                        }
                        term++;

                        if(term == SENDSIZE)
                        {
                            qDebug()<<"SEND";
                           // emit sendData(SENDSIZE);
                            emit sendDataBegin(&sendQueue,demoduType);
                            term = 0;
                        }

                }

            }




        }

        qDebug() << "stop demodulation" << endl;
    }

}

void Demodulation::ReadFilterCoeff(float *coeff,float *LPFcoeff, string hpcutoff, string lpcutoff)
{
    QString fileName = QString::fromLocal8Bit(hpcutoff.data());
    QString fileName2 = QString::fromLocal8Bit(lpcutoff.data());

    //fileName = QString("ButtorWorthFilterCoefficient_")+QString::number((int)(frequency/1000))+QString("KHz_50Hz.bin");
    //fileName = QString("ChebysheyIIFilterCoefficient_Order8_4Section_")+QString::number((int)(frequency/1000))+QString("KHz_1Hz.bin");//1Hz的高通滤波器文件名为ButtorWorthFilterCoefficient_Order8_1KHz_1Hz
    //fileName2 = QString("LPFilterCoefficient_")+QString::number((int)(frequency/1000))+QString("KHz_400Hz.bin");// 400Hz低通滤波器文件名LPFilterCoefficient_1KHz_400Hz.bin

    QString pat = QString("C:/DAS/ButtorWorthFilterCoefficient/") + fileName;
    QString pat2 = QString("C:/DAS/ButtorWorthFilterCoefficient/") + fileName2;

    qDebug()<<"Filter Coefficient path:"<<pat.toStdString().c_str()<<"  "<<pat2.toStdString().c_str();

    FILE *pFile = fopen(pat.toStdString().c_str(),"rb");
    FILE *pFile2 = fopen(pat2.toStdString().c_str(),"rb");

    if(pFile == NULL)
    {
        char logStr[64];
        sprintf(logStr,"Open %s Failed!.",pat.toStdString().c_str());
        LOG(logStr,"C:/DAS/log.txt");\
        QString info = QString("open failed :") + pat;
        emit sendShowQString(info);

    }
    else if(pFile2 == NULL)
    {
        char logStr[64];
        sprintf(logStr,"Open %s Failed!.",pat2.toStdString().c_str());
        LOG(logStr,"C:/DAS/log.txt");
        QString info = QString("open failed :") + pat;
        emit sendShowQString(info);
    }
    else{
    //QMessageBox::information(NULL,"ButtorWorthFilterCoefficient",pat.toStdString().c_str());
    fread(coeff,sizeof(float),FILTERODR,pFile);
    fread(LPFcoeff,sizeof(float),LPFILTERODR,pFile2);
    qDebug()<<"ReadFilterCoeff&LPFilterCoeff ok";
    for(int i = 0 ; i < FILTERODR; i++)
        qDebug() << "coeff[" << i << "] = " << coeff[i] << endl;

//    for(int i = 0 ; i < LPFILTERODR; i++)
//        qDebug() << "LPFcoeff[" << i << "] = " << LPFcoeff[i] << endl;

    }
    fclose(pFile);
    fclose(pFile2);
}
//该函数有bug
void Demodulation::FilterCoeffCalculate(float *coeff){//fre=5,res=1.5,FilterOrder=5,calculate buttorworthFilte coeff
    float res=1.5;
    float c=tan(M_PI*fre/frequency);
    coeff[0]=1/(1+res*c+pow(c,2));
    coeff[1]=(-2)*coeff[0];
    coeff[2]=coeff[0];
    coeff[3]=2*(pow(c,2)-1)*coeff[0];
    coeff[4]=(1-res*c+pow(c,2))*coeff[0];
}

void Demodulation::FreeMemory()
{
    delete[] RECORD_BUF;
    delete[] output;
    delete[] CH1Data;
    delete[] CH2Data;
    delete[] CH3Data;

    if(calcPhase)
    {
        delete[] Vi;
        delete[] Vq;
        delete[] Ph;
    }
    if(isFilter)
    {
        delete[] FilterCoeff;
        delete[] LPFilterCoeff;
        delete[] FirstRealPh;
        delete[] FirstIn_n;
        delete[] RealPhReg;
        delete[] RealPhOut;
        delete[] FilterReg;
        delete[] cnt;
        delete[] atanTable;
    }
    if(Unwraping){

        delete[] RealPh;
        delete[] PriorPh;
        delete[] K;
        delete[] PriorK;
    }
//    if(UnwrapFilter)
//    {
//        delete[] unwrapFilReg;
//    }

}

bool Demodulation::openDevice()
{
    bool isopen = false;
    if(USB->USBStart() == -1)
        isopen = true;

    return isopen;
}
