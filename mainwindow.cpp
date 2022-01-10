#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <QMessageBox>
#include <QThreadPool>
#include <QFile>
#include <QString>
#include <String>
#include <iostream>
#include <string.h>
#include <iterator>
#include "windows.h"
#include "mmsystem.h"
#include "wzserialport.h"
#pragma comment("winmm.lib")
#pragma comment("gdi32.lib")

#define DEBUGMODE 1
#define RUNMODE 0

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowFlags(Qt::WindowStaysOnTopHint);//主窗口保持顶层
    ui->setupUi(this);
    ui->save_checkBox->setEnabled(false);
    hWnd = (HWND)this->winId();


    string path = "C:/DAS/Config.txt";
    cfg = make_shared<Config>();
    cfg->Init(path);
    getcfg = make_shared<GetConfig>(*cfg);
    MinWignoreDevice = getcfg->getConfig_Debug();
    CaculationPhase = getcfg->getConfig_calcPhase();
    CHDataQue = make_shared<CirQueue<float>>(getcfg->getConfig_freqency()* 3 * 600);//循环队列长度为原始通道数据10min的数据量
    demoduDataQue = make_shared<CirQueue<float>>(getcfg->getConfig_freqency()* 3 * 600);//循环队列长度为原始通道数据10min的数据量
    WaveDataQue = make_shared<CirQueue<float>>(getcfg->getConfig_freqency()* 3 * 600);
    setSENDSIZE( (int)(getcfg->getConfig_freqency() / 4) );//根据采样率初始化单次发送数据包大小
    if( MinWignoreDevice == true )
    {
        string debugpath = "C:/DAS/debug.txt";
        getcfg->myConfig.InitDebug(debugpath);

        PostMessage(hWnd, SERIAL_WRITE_CONFIG_COMMAND_FINISHED, 0, 0);
        qDebug() << "ignore peakInit and configDevice" << endl;

        QString info1 = QString("Program Debugmode !!!!!");
        ui->StateText->append(info1);

        QString info2 = QString("ignore peakInit and conficgDevice");
        ui->StateText->append(info2);
    }
    else
    {

        peak = new PeakSearch(getcfg, hWnd);
        //peak->Init(getcfg->getPointoConfig(),hWnd);
        //peak->ConfigDevice();
        is_peakExist = true;
    }


}

MainWindow::~MainWindow()
{


    delete ui;
    qDebug() << "MainWindow deconstruct start" << endl;
    if(plt != NULL)
    {
        qDebug() << "Plot deconstruct start" << endl;
        plt->~Plot();
        qDebug() << "Plot deconstruct end" << endl;
    }

    if(Demodu != NULL)
    {
        qDebug() << "Demodu deconstruct start" << endl;
        Demodu->quit();
        Demodu->wait();
        Demodu->~Demodulation();
        qDebug() << "Demodu deconstruct end" << endl;
    }
    if(UDP != NULL)
    {
        qDebug() << "UDP deconstruct start" << endl;
        delete UdpThread;
        UdpThread->quit();
        UdpThread->wait();
        UDP->~UDPConnect();
        qDebug() << "UDP deconstruct end" << endl;
    }

    if(waveWidget != NULL)
    {
        qDebug() << "waveWidget deconstruct start" << endl;
        waveWidget->~WaveForm();
        qDebug() << "waveWidget deconstruct end" << endl;
    }
    qDebug() << "MainWindow deconstruct end" << endl;
}

bool MainWindow::setShow()
{
    return getcfg->myConfig.m_Wavesp->getIsWave();
}

int& MainWindow::setSENDSIZE(int sendsize)
{
    SENDSIZE = sendsize;
    return SENDSIZE;
}

void MainWindow::on_Eazystart_Button_clicked()
{
    //关闭无关按键显示
    ui->Demodu_Button->setEnabled(false);
    ui->SearchPeak_Button->setEnabled(false);
    ui->WatchPeak_Button->setEnabled(false);
    ui->DemoduStop_Button->setEnabled(false);
    //ui->Sound_Button->setEnabled(false);
    //i->Play_Button->setEnabled(false);
    qDebug() << "Eazystart_Button_clicked" << endl;
    if(MinWignoreDevice == true)
    {
        qDebug() << "MinWindow ignore RearchPeak and ConfirmPeak" << endl;
        QString info = QString("MinWindow ignore RearchPeak and ConfirmPeak");
        ui->StateText->append(info);

        debugDemudu(1);
    }
    else
    {
        //实现查看峰值功能（关闭显示）
        if(plt == NULL)
        {
            plt = new Plot();
            connect(plt,SIGNAL(ResearchPeakSignal()),this,SLOT(ResearchPeak()));
        }

        //实现确认峰值按键功能
        ConfirmPeakResult();
        qDebug()<<"ConfirmPeak"<<endl;

        connect(this, &MainWindow::sendPeakPosDone, this, &MainWindow::on_Demodu_Button_clicked);
    }

    QString info = QString("Version Inmormation: v0.4.4 \n") + QDateTime::currentDateTime().toString("yyyy-MM-dd\t") + SystemTime.toString()+QString("\teazystart finish, demodulation running...");
    ui->StateText->append(info);

}


void MainWindow::on_SearchPeak_Button_clicked()//开始寻峰 按钮按下
{
    SystemTime = QTime::currentTime();
    QString info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t") + SystemTime.toString()+QString("\tStart SearchPeak...");
    ui->StateText->append(info);
    connect(peak,SIGNAL(DataLenErrorSignal()),this,SLOT(ErrorWarnings()),Qt::QueuedConnection);
    peak->start();
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    QString info;
    if (eventType == "windows_generic_MSG")
    {
        PMSG msg = (PMSG)message;
        switch(msg->message)
        {
        case USB_FAILED:
            SystemTime = QTime::currentTime();
            info.clear();
            info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t")+SystemTime.toString()+QString("\tUSB_Start_Failure, Please confirm USB is occupied! ");
            ui->StateText->append(info);
            break;
        case SERIAL_WRITE_CONFIG_COMMAND_FAILED:
            SystemTime = QTime::currentTime();
            info.clear();
            info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t")+SystemTime.toString()+QString("\tSerial Write Command Failed!");
            ui->StateText->append(info);
            break;
        case SERIAL_WRITE_PEAKPOS_FAILED:

            SystemTime = QTime::currentTime();
            info.clear();
            info = QDateTime::currentDateTime().toString("yyyy-MM-dd")+SystemTime.toString()+QString("\tSerial Write PEAKPOS Failed!");
            ui->StateText->append(info);
            break;
        case SEARCH_PEAK_FINISHED:

            SystemTime = QTime::currentTime();
            info.clear();
            info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t")+SystemTime.toString()+QString("\tSearch Peak Succeed\n\t\tPlease Confirm Search Peak Result!");
            ui->StateText->append(info);

            break;
        case SERIAL_WRITE_CONFIG_COMMAND_FINISHED:

            SystemTime = QTime::currentTime();
            info.clear();
            info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t")+SystemTime.toString()+QString("\tSerial Write Command Successfully!");

            ui->StateText->append(info);
            break;
        }
    }
    return false;
}

void MainWindow::on_WatchPeak_Button_clicked()//查看峰值 按钮按下
{
    if(plt == NULL)
    {
        plt = new Plot();
        //plt->Init(peak);
        connect(plt,SIGNAL(ResearchPeakSignal()),this,SLOT(ResearchPeak()));
        connect(plt,&Plot::ConfirmSearchPeakSignal,this,&MainWindow::ConfirmPeakResult);
    }
    plt->show();
}


void MainWindow::ResearchPeak()
{
    SystemTime = QTime::currentTime();
    QString info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t") + SystemTime.toString()+QString("\tSerach Peak again...");
    ui->StateText->append(info);
    peak->ConfigDevice();
    peak->start();
}

void MainWindow::ErrorWarnings()
{
    peak->USB->USBClose();
    peak->wait();
    peak->start();
}

void MainWindow::ConfirmPeakResult()
{
    peak->readTxt2Peak();
    if(this->SendPeakPosTimer == NULL)
    {
        SendPeakPosTimer = new QTimer();
        sendPeakChannel = 0;
        peak->SendPeakPos(sendPeakChannel);
        SendPeakPosTimer->start(1000);//从peak.txt中读取的时间差
        connect(SendPeakPosTimer,&QTimer::timeout,this,&MainWindow::sendPeakPosData);
    }
    peak->savePeakNum(getcfg->getPointoConfig(), MinWignoreDevice);//从peakpos.txt中获取peaknum

    SystemTime = QTime::currentTime();

}

void MainWindow::showSendText(int sendsize)
{
    QString info = QString("send ") + QString::number(sendsize) + QString(" X 3CHdate");
    ui->StateText->append(info);
}

void MainWindow::showText(QString text)
{
    qDebug() << "showText" << text << endl;
    ui->StateText->append(text);
}

void MainWindow::textbrowserShowSpeed(double speed)
{
    QString speedtext = QString::number(speed, 'g', 6) + "Mbps";
    ui->textBrowser->setText(speedtext);
}

void MainWindow::sendPeakPosData()
{
    if(peak->Series->isSendCommandSuccessful(4))
    {
        sendPeakChannel++;
        if(sendPeakChannel < 3)
            peak->SendPeakPos(sendPeakChannel);
        qDebug()<<"sendPeakChannel  = "<<sendPeakChannel;
    }
    else
    {
        peak->SendPeakPos(sendPeakChannel);
    }

    if(sendPeakChannel == 3){
        SendPeakPosTimer->stop();
        QString info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t") + SystemTime.toString()+QString("\tSend PeakPos Successfully!");
        ui->StateText->append(info);

        emit sendPeakPosDone();
    }
}

void MainWindow::isShowWindow(Demodulation *demodu)
{
    if(setShow())
    {
        //初始化波形显示窗口
        //wgt = new MainWidget(SENDSIZE, demodu->frequency / 1000);
        //wgt->initUI(demodu->peakNum);
        //connect(demodu, SIGNAL(sendWaveData(CirQueue<float>*,int)), wgt, SLOT(updateData(CirQueue<float>*, int)) );
        //wgt->show();
    }
}

void MainWindow::debugDemudu(bool)
{
    /* 按键初始化使能
     * */
    ui->save_checkBox->setEnabled(true);
    ui->SearchPeak_Button->setEnabled(false);
    SystemTime = QTime::currentTime();
    QString info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t") + SystemTime.toString()+QString("\tStart Demodulation...");
    ui->StateText->append(info);
    ui->Demodu_Button->setEnabled(false);
    ui->DemoduStop_Button->setEnabled(true);

    /* 初始化接收数据成员
     * */
    RVD = make_shared<RecvData>(getcfg, CHDataQue);
    connect(RVD.get(), SIGNAL(sendSpeed(double)), this, SLOT(textbrowserShowSpeed(double)));

    /* 初始化解调成员
     * */
    Demodu = new Demodulation(true, getcfg, CHDataQue, demoduDataQue, WaveDataQue);
    connect(Demodu, SIGNAL( sendShowQString(QString) ), this, SLOT(showText(QString)) );

    /* 初始化存储数据成员
     * */
    UDP = new UDPConnect(getcfg, demoduDataQue);

    wgt = new MainWidget(getcfg, WaveDataQue);
    wgt->initUI();

    RVD->start();//开启接收数据线程
    Demodu->start();//开启解调线程
    wgt->show();
}

void MainWindow::debugDemudu()//debug模式进入demodulation
{
    ui->save_checkBox->setEnabled(true);
    ui->SearchPeak_Button->setEnabled(false);
    SystemTime = QTime::currentTime();
    QString info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t") + SystemTime.toString()+QString("\tStart Demodulation...");
    ui->StateText->append(info);
    ui->Demodu_Button->setEnabled(false);
    ui->DemoduStop_Button->setEnabled(true);
    Demodu = new Demodulation(hWnd, setShow());
    connect(Demodu, SIGNAL( sendShowQString(QString) ), this, SLOT(showText(QString)) );
    Demodu->Init(getcfg->getConstPointoConfig(), DEBUGMODE);
    connect(Demodu, SIGNAL(sendSpeed(double)), this, SLOT(textbrowserShowSpeed(double)));
    wavFre = Demodu->frequency;

    waveWidget = new WaveForm();
    waveWidget->WidgetInit();
    waveWidget->paramInit(getcfg->getConstPointoConfig());

    UDP = new UDPConnect(getcfg->getConstPointoConfig(), SENDSIZE);

    //UdpThread = new QThread();
    //UdpThread1 = new QThread();

    //UDP->initSeriesParam(waveWidget->lineSeries_1,waveWidget->lineSeries_2,waveWidget->lineSeries_3);

    //UDP->moveToThread(UdpThread);


    //QObject::connect(waveWidget,SIGNAL(SelectFBGChanged(int,int,int)),UDP, SLOT(changeSelectFBG(int,int,int)));


    qRegisterMetaType<CirQueue<float>>("CirQueue<float>");
    qRegisterMetaType<CirQueue<float>>("CirQueue<float>*");

    QObject::connect(Demodu,SIGNAL(sendDataBegin(CirQueue<float>*,char)),
                     UDP,SLOT(executeSendData(CirQueue<float>*,char)));
    connect(Demodu, SIGNAL(sendData(int)), this, SLOT(showSendText(int)));

    //初始化波形显示窗口
    isShowWindow(Demodu);
    Demodu->start();
    UdpThread->start();

}

void MainWindow::on_Demodu_Button_clicked()//开始解调 按键按下
{
    if (is_peakExist)
    {
        peak->~PeakSearch();
        is_peakExist = false;
    }

    ui->save_checkBox->setEnabled(true);
    ui->SearchPeak_Button->setEnabled(false);
    SystemTime = QTime::currentTime();
    QString info = QDateTime::currentDateTime().toString("yyyy-MM-dd\t") + SystemTime.toString()+QString("\tStart Demodulation...");
    ui->StateText->append(info);
    ui->Demodu_Button->setEnabled(false);
    ui->DemoduStop_Button->setEnabled(true);

    /* 初始化接收数据成员
     * */
    RVD = make_shared<RecvData>(getcfg, CHDataQue);
    connect(RVD.get(), SIGNAL(sendSpeed(double)), this, SLOT(textbrowserShowSpeed(double)));

    /* 初始化解调成员
     * */
    Demodu = new Demodulation(true, getcfg, CHDataQue, demoduDataQue, WaveDataQue);
    connect(Demodu, SIGNAL( sendShowQString(QString) ), this, SLOT(showText(QString)) );

    /* 初始化存储数据成员
     * */
    UDP = new UDPConnect(getcfg, demoduDataQue);

    wgt = new MainWidget(getcfg, WaveDataQue);
    wgt->initUI();

    RVD->start();//开启接收数据线程
    Demodu->start();//开启解调线程
    //UDP->start();//开启存储线程
    wgt->show();
//    Demodu = new Demodulation(hWnd, setShow());
//    connect(Demodu, SIGNAL( sendShowQString(QString) ), this, SLOT(showText(QString)) );
//    Demodu->setSENDSIZE(SENDSIZE);
//    Demodu->Init(getcfg->getPointoConfig(), RUNMODE);
//    QString info1 = QString("Demodulation Init done!!   Start Demodu !!");
//    ui->StateText->append(info1);
//    connect(Demodu, SIGNAL(sendSpeed(double)), this, SLOT(textbrowserShowSpeed(double)));
//    wavFre = Demodu->frequency;

//    waveWidget = new WaveForm();
//    waveWidget->WidgetInit();
//    waveWidget->paramInit(getcfg->getConstPointoConfig());

//    UDP = new UDPConnect(getcfg->getConstPointoConfig(), SENDSIZE);

//    UdpThread = new QThread();
//    //UdpThread1 = new QThread();

//    UDP->initSeriesParam(waveWidget->lineSeries_1,
//                         waveWidget->lineSeries_2,
//                         waveWidget->lineSeries_3);

//    UDP->moveToThread(UdpThread);


//    QObject::connect(waveWidget,SIGNAL(SelectFBGChanged(int,int,int)),
//             UDP, SLOT(changeSelectFBG(int,int,int)));


//    qRegisterMetaType<CirQueue<float>>("CirQueue<float>");
//    qRegisterMetaType<CirQueue<float>>("CirQueue<float>*");
//    QObject::connect(Demodu,SIGNAL(sendDataBegin(CirQueue<float>*,char)),
//                     UDP,SLOT(executeSendData(CirQueue<float>*,char)));

//    connect(Demodu, SIGNAL(sendData(int)), this, SLOT(showSendText(int)));

//    //初始化波形显示窗口
//    //初始化波形显示窗口
//    isShowWindow(Demodu);
//    Demodu->start();
//    UdpThread->start();


}

void MainWindow::on_DemoduStop_Button_clicked()
{


    if(wgt != NULL)
    {
        wgt->close();
        delete wgt;
    }
    RVD->stopRecvData();
    RVD->wait();

    UDP->saveTimer.stop();
    UDP->is_saveData = false;
    emit stopSave();//发出UDP停止存储信号
    Sleep(10);//等待，确保UDP存储数据的函数中退出
    delete UDP;
    UDP = NULL;

    Demodu->stopDemodulation();
    Demodu->wait();

    delete Demodu;
    Demodu = NULL;




    ui->save_checkBox->setCheckState(Qt::Unchecked);//设置成未勾选
    ui->save_checkBox->setEnabled(false);
    ui->Demodu_Button->setEnabled(true);
    ui->DemoduStop_Button->setEnabled(false);

}

void MainWindow::on_Play_Button_clicked()
{
    Demodu->soundSave = false;
    QString region = ui->regionEidt->toPlainText();
    float *buf;
    float *bufAll;
    int regionNum = Demodu->peakNum;
    int vSize = Demodu->vSound.size();
    bufAll = new float[vSize];
    int len = vSize/regionNum;
    buf = new float[len];
    ui->StateText->append("wave len = " + QString::number(len));
    //cout << "total len = " << len << endl;
    vector<float>::iterator vb = Demodu->vSound.begin();
    vb += region.toInt() - 1;//
    for (int i = 0;i != len;i++)
    {
        buf[i] = *vb;
        vb += regionNum;//选区
    }



    /*--------------------------------------------保存bin-----------------------------------------*/
    FILE *file3;
    file3 = fopen("D:/DAS/test.bin","wb+");
    vb = Demodu->vSound.begin();
    for (int i = 0;i != vSize;i++)
    {
        bufAll[i] = 2*(*vb);//系数为声音播放功能的声音大小放大倍数
        ++vb;
    }
    fwrite(bufAll,sizeof(float),vSize,file3);
    fclose(file3);
    delete bufAll;


    /*--------------------------------------------配置wav的头-----------------------------------------*/
    head.RIFFSize = len*sizeof(float) + 48;

    head.FMTSize = 16;//fmt块长度
    head.AudioFormat = 3;
    head.SampleRate = wavFre;
    head.ByteRate = wavFre*sizeof(float);
    head.BlockAlign = 4;
    head.BitPerSample = 32;

    head.SamplePiontNum = len;//

    head.DATASize = len*sizeof(float);

    /*--------------------------------------------存入wav-----------------------------------------*/
    FILE *file2;
    string path0 = "D:/DAS/";
    QString path1 = QString(path0.c_str())+QString::number(region.toInt()) + QString(".wav");
    string path2 = path1.toStdString().c_str();

    file2 = fopen(path1.toStdString().c_str(),"wb+");
    if (file2 != NULL)
    {
        fwrite(&head,sizeof(head),1,file2);
        fwrite(buf,sizeof(float),len,file2);
    }
    fclose(file2);
    ui->StateText->append(path1+" save success!");
    delete buf;
    cout << "fre = " << wavFre << endl;

    const char *str = path2.c_str();
    wchar_t *a1 = c2w(str);
    LPCWSTR playPath = a1;
    PlaySound(playPath, NULL, SND_FILENAME | SND_ASYNC);
}

void MainWindow::on_save_checkBox_clicked()
{
    if (ui->save_checkBox->isChecked())
    {
        QDateTime systemDate = QDateTime::currentDateTime();
        QTime systemTime = QTime::currentTime();
        UDP->changeFileNameOnce(systemDate, systemTime);
        UDP->saveTimer.start(60000);//1min 新建一个bin文件
        QObject::connect(&UDP->saveTimer,&QTimer::timeout,UDP,&UDPConnect::getFilename);
        connect(this, SIGNAL(stopSave()), UDP, SLOT(stopSaveSlot()) );
        UDP->is_saveData = true;
    }
    else
    {
        UDP->saveTimer.stop();
        UDP->is_saveData = false;
        emit stopSave();//发出UDP停止存储信号
    }

}

void MainWindow::on_Sound_Button_clicked()
{
    vector<float>().swap(Demodu->vSound);
    Demodu->soundSave = true;
    ui->StateText->append("start save sound");

}

wchar_t* MainWindow::c2w(const char *str)
{
    int length = strlen(str)+1;
    wchar_t *t = (wchar_t*)malloc(sizeof(wchar_t)*length);
    memset(t,0,length*sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP,0,str,strlen(str),t,length);
    return t;
}



