#ifndef DEMODULATION_H
#define DEMODULATION_H
#include "USBCtrl.h"
#include <QThread>
#include <queue>
#include <windows.h>
#include "Config.h"
#include <math.h>
#include <string>
#include <QQueue>
#include <QMetaType>
#include <QMutex>
#include <vector>
#include <thread>
#include <algorithm>
#include <QObject>
#include <QRunnable>
#include <QThreadPool>
#include <QTime>
#include <fstream>
#include "PeakSearch.h"


#include "udpconnect.h"
#include "CirQueue.h"
#define FILTERODR 6
#define LPFILTERODR 10
#define SENDSIZE 9000
#define NUMTABLE 201

#define fre 5
#define FilterOrder 5

using namespace  std ;


class Demodulation : public QThread
{
    Q_OBJECT
public:
    Demodulation(HWND hWnd);

    Demodulation(HWND hWnd,Config *cfig);
    ~Demodulation();

    void demoduPhase();

    void Init(Config *cfig, int mode);

    void debugInit(Config *cfig);

    void ReadFilterCoeff(float *coeff,float *LPFcoeff, string hpcutoff, string lpcutoff);
    void FilterCoeffCalculate(float *coeff);

    void FreeMemory();

    void run();

    bool openDevice();
    float demoduPh(float vi,float vq);

    void readAtanTable(float *roundNum);
    void stopDemodulation();

//滤波器相关
    float IIR_DR(float x, float* plast,  float* A,  float* B);
    float ChebysheyII_HP_Filter(float originalph, int num);

    void Hpfilter(int i);


    QString path[10];
    int frequency;

    //debug
    void debugRunDemodu();
    vector<float> debugReadData(QString filename, bool &filend, int &pos);
    void debugdemoduPhase(int vectornum);


signals:
    void sendData(int sendsize);
    void sendDataBegin(CirQueue<float> *que, char Type);
    void sendShowQString(QString str);
    void sendWaveData(CirQueue<float> *out, int peaknum);
    void sendSpeed(double);



private:

    USBCtrl *USB;
    QMutex m_lock;

    bool calcPhase;
    bool Unwraping;
    bool isFilter;
    bool isLPFilter;
    //bool UnwrapFilter;
    bool isUSBOpen;

    bool DebugDemodu;

    bool is_demoduRun;

    UDPConnect *UDP;
    QThread *m_Thread;

    HWND m_hWnd;

    QTimer *timer2;

    int DemoduNum;

    void getCHdata(int &i, int &j, int &num);

    inline void showDemoduImformation(Config *);

public:



    int peakNum;
    int CHdatalen=0;
    int Mode;
    unsigned short* CH1Data;
    unsigned short* CH2Data;
    unsigned short* CH3Data;

    vector<float> debugCH1Data;
    vector<float> debugCH2Data;
    vector<float> debugCH3Data;

    unsigned long *cnt;


    int *FirstIn_n;

    float *Vi;
    float *Vq;
    float *K;
    float *PriorK;
    float *Ph;
    float *RealPh;
    float *PriorPh;
    float *FirstRealPh;
    float (*xlast)[2];
    float (*mlast)[2];
    float (*mlast2)[2];
    float (*mlast3)[2];
    float (*RealPhReg)[FILTERODR];
    float (*RealPhOut)[FILTERODR];
    float (*FilterReg)[LPFILTERODR];

    float *output;
    float *waveout;
    float *atanTable;
    float *FilterCoeff;
    float *LPFilterCoeff;



    BYTE* RECORD_BUF;
    bool demoduStop;

    CirQueue<float> sendQueue;
    CirQueue<float> DisplayQueue;


    int flag=0;

    char demoduType;
    string hpcutoff;
    string lpcutoff;
    bool soundSave;

    vector<float> vSound;

private slots:
    void timerSlot2();

};

#endif // DEMODULATION_H
