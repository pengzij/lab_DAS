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
#include "PeakSearch.h"


#include "udpconnect.h"
#include "CirQueue.h"
#define FILTERODR 6
#define LPFILTERODR 10
#define SENDSIZE 90000
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

    void Init(Config *cfig);
    void ReadFilterCoeff(float *coeff,float *LPFcoeff);
    void FilterCoeffCalculate(float *coeff);

    void FreeMemory();

    void run();

    bool openDevice();
    float demoduPh(float vi,float vq);

    void readAtanTable(float *roundNum);
    void stopDemodulation();

    QString path[10];
    int frequency;
signals:
    void sendData();
    void sendDataBegin(CirQueue<float> *que, char Type);



private:

    USBCtrl *USB;
    QMutex m_lock;

    bool calcPhase;
    bool Unwraping;
    bool isFilter;
    bool isLPFilter = true;
    bool UnwrapFilter;
    bool isUSBOpen;
    bool is_demoduRun;

    UDPConnect *UDP;
    QThread *m_Thread;

    HWND m_hWnd;

public:



    int peakNum;
    int CHdatalen=0;

    unsigned short* CH1Data;
    unsigned short* CH2Data;
    unsigned short* CH3Data;
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
    float (*RealPhReg)[FILTERODR];
    float (*RealPhOut)[FILTERODR];
    float (*FilterReg)[LPFILTERODR];
    float (*unwrapFilReg)[3];
    float *output;
    float *atanTable;
    float *FilterCoeff;
    float *LPFilterCoeff;



    BYTE* RECORD_BUF;
    bool demoduStop;

    CirQueue<float> sendQueue;
    CirQueue<float> DisplayQueue;

    int flag=0;

    char demoduType;
    bool soundSave;

    vector<float> vSound;

};

#endif // DEMODULATION_H
