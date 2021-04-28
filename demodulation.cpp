#include "demodulation.h"
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

    if(cfig->m_demodulation->m_UnwrapFilt == std::string("true"))
        this->UnwrapFilter = true;
    else if(cfig->m_demodulation->m_UnwrapFilt == std::string("false"))
        this->UnwrapFilter = false;
    else{}

    peakNum = cfig->m_demodulation->m_peakNum;
    frequency = cfig->m_FPGACard->m_freq;
    demoduStop = false;
    soundSave = false;

    if(UnwrapFilter){
        unwrapFilReg = new float[peakNum][3];
    }

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
                FirstRealPh=new float[peakNum]();
                RealPhReg = new float[peakNum][FILTERODR]();
                RealPhOut = new float[peakNum][FILTERODR]();
                FilterReg = new float[peakNum][LPFILTERODR]();
                cnt=new unsigned long[peakNum]();//buttorworth函数执行次数
                FirstIn_n=new int[peakNum]();
                FilterCoeff = new float[5]();
                LPFilterCoeff =new float[10]();
                ReadFilterCoeff(FilterCoeff,LPFilterCoeff);

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
            Vq[i] = -sqrt(3.0) * (CH1Data[i] - CH2Data[i]);

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
                PriorK[i]=K[i];

                if(isFilter)//buttorworth
                {
                    if(FirstIn_n[i]==0){
                        FirstRealPh[i]=RealPh[i];
                        FirstIn_n[i]=1;
                    }

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
                    }

                    if(isLPFilter){
                        FilterReg[i][0]=LPFilterCoeff[0]*(RealPhOut[i][2]-LPFilterCoeff[1]*FilterReg[i][1]-LPFilterCoeff[2]*FilterReg[i][2]-LPFilterCoeff[3]*FilterReg[i][3]-LPFilterCoeff[4]*FilterReg[i][4]);
                        output[i] = LPFilterCoeff[5]*FilterReg[i][0]+LPFilterCoeff[6]*FilterReg[i][1]+LPFilterCoeff[7]*FilterReg[i][2]+LPFilterCoeff[8]*FilterReg[i][3]+LPFilterCoeff[9]*FilterReg[i][4];
                        for(int n=4;n>=1;n--){
                            FilterReg[i][n]=FilterReg[i][n-1];
                            }
                    }
                    else{
                         output[i] = RealPhOut[i][2];
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


void Demodulation::run()
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
                        emit sendDataBegin(&sendQueue,demoduType);
                        term = 0;
                    }

            }

        }
    }

    qDebug() << "stop demodulation" << endl;
}

void Demodulation::ReadFilterCoeff(float *coeff,float *LPFcoeff)
{
    QString fileName;
    QString fileName2;

    fileName = QString("ButtorWorthFilterCoefficient_")+QString::number((int)(frequency/1000))+QString("KHz_150Hz.bin");
    fileName2 = QString("LPFilterCoefficient_")+QString::number((int)(frequency/1000))+QString("KHz_1.5KHz.bin");

    QString pat = QString("C:/DAS/ButtorWorthFilterCoefficient/") + fileName;
    QString pat2 = QString("C:/DAS/ButtorWorthFilterCoefficient/") + fileName2;

    qDebug()<<"Filter Coefficient path:"<<pat.toStdString().c_str()<<"  "<<pat2.toStdString().c_str();

    FILE *pFile = fopen(pat.toStdString().c_str(),"rb");
    FILE *pFile2 = fopen(pat2.toStdString().c_str(),"rb");

    if(pFile == NULL)//如果没有读取到相应的滤波器参数，则跳转参数计算函数
    {
        char logStr[64];
        sprintf(logStr,"Open %s Failed!.",pat.toStdString().c_str());
        LOG(logStr,"C:/DAS/log.txt");
        //qDebug()<<"ReadFilterCoeffNULL. Start to calculate FilterCoff"<<endl;
        //FilterCoeffCalculate(coeff);

    }else{
    //QMessageBox::information(NULL,"ButtorWorthFilterCoefficient",pat.toStdString().c_str());
    fread(coeff,sizeof(float),FILTERODR,pFile);
    fread(LPFcoeff,sizeof(float),LPFILTERODR,pFile2);
    qDebug()<<"ReadFilterCoeff&LPFilterCoeff ok";
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
    if(UnwrapFilter)
    {
        delete[] unwrapFilReg;
    }

}

bool Demodulation::openDevice()
{
    bool isopen = false;
    if(USB->USBStart() == -1)
        isopen = true;

    return isopen;
}
