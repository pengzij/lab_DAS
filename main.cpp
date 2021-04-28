#include "mainwindow.h"
#include <QApplication>
#include "plot.h"
#include "waveform.h"
#include <QVector>
#include <QPointF>
#include <QString>
#include <QDateTime>
#include "Config.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

}
