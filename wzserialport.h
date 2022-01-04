#ifndef WZSERIALPORT_H
#define WZSERIALPORT_H

#include <QtSerialPort/QtSerialPort>
#include <QDebug>
#include <QString>
#include <Windows.h>
#include <QByteArray>
#include "Config.h"




class WzSerialPort: public QObject
{
public:
    WzSerialPort();

public:
    int WriteCommand(char CommandOdr, int* dataBuf, int dataLen,int Channel);
    int WriteCommand(char CommandOdr, shared_ptr<GetConfig> WPgcfg);

    bool isSendCommandSuccessful(char CommanOdr);

    bool Init(const Config *cfig);

    void readMyCom();
    QSerialPort *Serial;
private:
    QString PortName;
    uint BaudRate;
    QByteArray Recv;


};

#endif // WZSERIALPORT_H
