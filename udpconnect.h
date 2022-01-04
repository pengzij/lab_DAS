#ifndef UDPCONNECT_H
#define UDPCONNECT_H
#include <iostream>
#include <string>
#include <algorithm>
#include <QUdpSocket>
#include <QHostAddress>
#include "Config.h"
#include <queue>
#include <QQueue>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QBarSet>
#include <QBarSeries>
#include "CirQueue.h"
#include <QMetaType>
#include "waveform.h"
#include <QDateTime>
#include <QTime>
#include <QTimer>
#include <memory>

using namespace QtCharts;
using namespace std;

#define SEND_ORIGNAL_DATA               ((char)0x01)
#define SEND_NOT_UNWRAP_PHASE_DATA      ((char)0x02)
#define SEND_FILTED_PHASE_DATA          ((char)0x03)
#define SEND_UNFILT_PHASE_DATA          ((char)0x04)
#define SEND_PHASE_DATA                 ((char)0x05)

class UDPConnect :public QObject
{
    Q_OBJECT
public:
    UDPConnect() = default;
    UDPConnect(const Config *cfig, int& sendsize);
    UDPConnect(shared_ptr<GetConfig> gcfg, shared_ptr<CirQueue<float>> saveque);
    ~UDPConnect();
    long getHasSend(){
        return hasSend;
    }

    QHostAddress AddressIP;
    QUdpSocket *socket = NULL;
    int port;
    int PeakNum;
    int frequency;

    void changeFileNameOnce(QDateTime &systemDate, QTime &systemTime);
    int sendData(CirQueue<float>* que,char dataType);
    void loadSeriesData(char dataType);
    void initSeriesParam(QLineSeries *line1, QLineSeries *line2, QLineSeries *line3);

    void saveData2Bin(float* data, bool &now);
    void saveData2Bin(float *data);
    //int& setSENDSIZE(int &sendsize);

    QLineSeries *m_lineSeries_1;
    QLineSeries *m_lineSeries_2;
    QLineSeries *m_lineSeries_3;

    int selectFBG_1;
    int selectFBG_2;
    int selectFBG_3;

    float *QueData = NULL;
    //QByteArray *data = NULL;


    QVector<QPointF> points;

    char Head[8] = {(char)0xF1,(char)0xF2,(char)0xF3,(char)0xF4};

    long hasSend;

    string saveFolder;
    QString savePath;

    bool isSave;
    bool isSend;


    size_t waveLength;

    char DataType;

    QString *path;
    FILE **pFile=NULL;

    QTimer saveTimer;

    QTime systemTime;
    QDateTime systemDate;

    bool is_saveData = false;


private:
    shared_ptr<GetConfig> UDPgcfg;
    shared_ptr<CirQueue<float>> saveDataQue;
    QMutex writeLock;
    QString saveFilename1;
    QString saveFilename2;
    QString saveFilename3;
    QString *saveFilename;
    ofstream outfile1;
    ofstream outfile2;
    ofstream outfile3;
    shared_ptr<vector<ofstream>> outfile;
    int SENDSIZE;
    int filesize;
    bool hasNextFilename;
    unsigned long onceSaveNum;
    unsigned long saveNum;
    QTimer* udpTimer;


    void saveOriData(const unsigned long & num);
    void saveDemoduData(const unsigned long& num);

public slots:
    void executeSendData(CirQueue<float>* que, char Type);
    void changeSelectFBG(int FBG1,int FBG2, int FBG3);

    void changeFileName();
    void getFilename();
    void executeData();
    void stopSaveSlot();
    void checkSaveSlot();

signals:
    void sendNextFile();
};

#endif // UDPCONNECT_H
