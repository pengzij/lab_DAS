#ifndef UDPCONNECT_H
#define UDPCONNECT_H
#include <string>
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

using namespace QtCharts;
using namespace std;

#define SEND_ORIGNAL_DATA               ((char)0x01)
#define SEND_NOT_UNWRAP_PHASE_DATA      ((char)0x02)
#define SEND_FILTED_PHASE_DATA          ((char)0x03)
#define SEND_UNFILT_PHASE_DATA          ((char)0x04)
#define SEND_PHASE_DATA                 ((char)0x05)

class UDPConnect : public QObject
{
    Q_OBJECT
public:
    UDPConnect(Config *cfig);
    ~UDPConnect();
    long getHasSend(){
        return hasSend;
    }

public slots:
    void executeSendData(CirQueue<float>* que, char Type);
    void changeSelectFBG(int FBG1,int FBG2, int FBG3);

    void changeFileName();
    void executeData();
public:
    QHostAddress AddressIP;
    QUdpSocket *socket = NULL;
    int port;
    int PeakNum;
    int frequency;


    int sendData(CirQueue<float>* que,char dataType);
    void loadSeriesData(char dataType);
    void initSeriesParam(QLineSeries *line1, QLineSeries *line2, QLineSeries *line3);

    void saveData2Bin(float* data);

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
    QMutex writeLock;

};

#endif // UDPCONNECT_H
