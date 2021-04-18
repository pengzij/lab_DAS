#include "mainwindow.h"
#include <QApplication>
#include "plot.h"
#include "waveform.h"
#include <QVector>
#include <QPointF>
#include <QString>
#include <QDateTime>
#include "Config.h"
//2020.10.7.22.14  demodu change to extern,one thread to finish 21 CHannl,threadpool ok,down of 11k is right,up of it is wrong!
//udpconnect changefile() need change ,have wrong
//2020.10.8.8.33   fixed
int main(int argc, char *argv[])
{



    QApplication a(argc, argv);
    MainWindow w;
    w.show();


//    string path = "C:/DAS/Config.txt";
//    Config::instance()->Init(path);
//    WaveForm wf;
//    wf.paramInit(Config::instance());
//    wf.WidgetInit();
//    wf.show();
//   QVector<QPointF> point;
//    for(int i = 0; i<100;i++)
//        point.append(QPointF(i,i+5));

//    for(int i = 0;i<100;i++)
//        qDebug()<<point.at(i);
    //WaveForm form;
    //form.show();
    return a.exec();
    //return 0;
}
