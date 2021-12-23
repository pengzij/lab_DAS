#ifndef RECVDATA_H
#define RECVDATA_H
#include <iostream>
#include <QThread>
#include <queue>
#include "Config.h"
#include <string>
#include <QMutex>
#include <thread>
#include <QRunnable>
#include <memory>
#include "Config.h"
#include "USBCtrl.h"

using namespace std;

class RecvData: public QThread
{
    Q_OBJECT
public:
    RecvData() = default;
    RecvData(Config* config, queue<float>& que);
    ~RecvData();

private:
    size_t getUSBData();


    queue<float> CHdata;

};


#endif // RECVDATA_H
