#ifndef RECVDATA_H
#define RECVDATA_H
#include <iostream>
#include <QThread>
#include <queue>
#include <string>
#include <QMutex>
#include <thread>
#include <QRunnable>
#include <memory>
#include <QTimer>
#include <QFile>
#include "CirQueue.h"
#include "Config.h"
#include "USBCtrl.h"

using namespace std;

class RecvData: public QThread
{
    Q_OBJECT
public:
    RecvData() = default;
    RecvData(const shared_ptr<GetConfig> gcfg, shared_ptr<CirQueue<float>> que);
    ~RecvData();

    void run() override;
    void stopRecvData();

private:
    size_t getUSBData();
    void debugReadData(QString& filename, bool &filend, int &pos, shared_ptr<vector<float>>& debugCHdata);
    void debugRecvData();
    void RecvDataRun();
    void getDatafromByteTofloat(int &i, int &j, int &num);

    shared_ptr<CirQueue<float>> CHdata;
    const int peakNum;
    shared_ptr<GetConfig> RDgcfg;
    bool isDebug;
    shared_ptr<USBCtrl> USB;
    shared_ptr<BYTE*> RECORD_BUF;
    shared_ptr<QTimer> timer;
    int hasRecvNum;

    bool is_RecvRun;
    shared_ptr<vector<float>> debugCH1Data;
    shared_ptr<vector<float>> debugCH2Data;
    shared_ptr<vector<float>> debugCH3Data;



signals:
    void sendSpeed(double);

private slots:
    void timerSlot();
};


#endif // RECVDATA_H
