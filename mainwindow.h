#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QTime>
#include <QMetaType>
#include <QSound>
#include <QString>
#include "WavHead.h"
#include "demodulation.h"
#include "udpconnect.h"
#include "CirQueue.h"
#include "waveform.h"
#include "mainwidget.h"
#include "wzserialport.h"
#include "PeakSearch.h"
#include "plot.h"
#include "recvdata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

    WavHead head;
    QSound *sound = NULL;
    QString wavPath[10];
    int wavFre;
    int& setSENDSIZE(int);


private slots:
    void on_SearchPeak_Button_clicked();

    void on_WatchPeak_Button_clicked();

    void ResearchPeak();

    void ErrorWarnings();

    void ConfirmPeakResult();

    void sendPeakPosData();

    void on_Demodu_Button_clicked();


    void on_DemoduStop_Button_clicked();

    void on_Play_Button_clicked();

    void on_save_checkBox_clicked();

    void on_Sound_Button_clicked();

    void on_Eazystart_Button_clicked();

    void showSendText(int sendsize);

    void showText(QString text);

    void textbrowserShowSpeed(double );



private:
    Ui::MainWindow *ui;
    HWND hWnd;
    shared_ptr<Config> cfg;
    shared_ptr<GetConfig> getcfg;
    WzSerialPort *serialport;
    PeakSearch *peak;

    Plot *plt = NULL;
    MainWidget *wgt = NULL;

    shared_ptr<RecvData> RVD;

    Demodulation *Demodu = NULL;
    Demodulation *Demodu_2=NULL;
    Demodulation *Demodu_3=NULL;

    QTimer *SendPeakPosTimer = NULL;
    int sendPeakChannel;
    UDPConnect *UDP = NULL;
    QThread *UdpThread;
    QThread *UdpThread1;
    QTime SystemTime;

    WaveForm *waveWidget = NULL;

    bool is_peakExist = false;
    wchar_t* c2w(const char* str);
    bool MinWignoreDevice = false;

    unsigned short saveCounter = 0;
    bool CaculationPhase;
    inline void isShowWindow(Demodulation*);
    int SENDSIZE;
    shared_ptr<CirQueue<float>> CHDataQue;
    shared_ptr<CirQueue<float>> demoduDataQue;
    shared_ptr<CirQueue<float>> WaveDataQue;

    void debugDemudu();
    void debugDemudu(bool);
    bool setShow();


signals:
    void sendPeakPosDone();
    void stopSave();
};

#endif // MAINWINDOW_H
