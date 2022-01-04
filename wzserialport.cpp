#include "wzserialport.h"

WzSerialPort::WzSerialPort()
{

}


bool WzSerialPort::Init(const Config *cfig)
{
    bool openSuc = false;
    int COM = cfig->m_FPGACard->m_portName;
    this->PortName = QString("COM") + QString::number(COM);
    //qDebug<<this->PortName;
    this->Serial = new QSerialPort(PortName);

    BaudRate= cfig->m_FPGACard->m_baudRate;

    Serial->setBaudRate(BaudRate);

    Serial->setDataBits(QSerialPort::Data8);

    Serial->setParity(QSerialPort::NoParity);

    Serial->setStopBits(QSerialPort::OneStop);

    Serial->setFlowControl(QSerialPort::NoFlowControl);

    if(Serial->isOpen())
    {
        Serial->clear();
        Serial->close();
    }
    openSuc = Serial->open(QIODevice::ReadWrite);

    QObject::connect(Serial,&QSerialPort::readyRead,this,&WzSerialPort::readMyCom);

    Recv.clear();

    return openSuc;
}

void WzSerialPort::readMyCom(){
     Recv.append(Serial->readAll());
     qDebug()<< Recv;
}


/****************************************************************/
/*Write Command To FPGA                                       　*/
/*commOdr = 2  Represent Init FPGA                           　 */
/*commOdr = 3  Represent Start FPGA                             */
/*commOdr = 4  Represent Start FPGA                             */
/****************************************************************/
int WzSerialPort::WriteCommand(char CommandOdr, shared_ptr<GetConfig> WPgcfg) {

    int hasWrite = 0;
    int fre = WPgcfg->getConfig_freqency();
    int channelLen = WPgcfg->getConfig_channelLength();
    QByteArray comm;

    switch (CommandOdr)
    {
    case 2:

        comm.resize(30);

        comm[0] = 0xAA; comm[1] = 0x55; comm[2] = 0x7E; comm[3] = 0x01;

        comm[4] = 0x00; comm[5] = 0x02;

        comm[6] = 0x11; comm[7] = 0x11; comm[8] = 0x11; comm[9] = 0x11;

        comm[10] = 0x22; comm[11] = 0x22;

        comm[12] = (fre >> 8) & 0xFF;

        comm[13] = (BYTE)(fre) & 0xFF;

        comm[14] = 0x00; comm[15] = 0x0A;

        comm[16] = 0x00; comm[17] = 0x03;

        comm[18] = (channelLen >> 24) & 0xFF;
        comm[19] = (channelLen >> 16) & 0xFF;
        comm[20] = (channelLen >> 8) & 0xFF;
        comm[21] = (channelLen ) & 0xFF;

        comm[22] = 0x00; comm[23] = 0x00;

        comm[24] = 0x00; comm[25] = 0x00;

        comm[26] = 0x00; comm[27] = 0xFF; comm[28] = 0xFF; comm[29] = 0x00;

        hasWrite = Serial->write(comm,30);

        Serial->waitForBytesWritten(10);

        break;

    case 3:

        comm.resize(16);

        comm[0] = 0xAA; comm[1] = 0x55; comm[2] = 0x7E; comm[3] = 0x01;

        comm[4] = 0x00; comm[5] = 0x03;

        comm[6] = 0x11; comm[7] = 0x11; comm[8] = 0x11; comm[9] = 0x11;

        comm[10] = 0x00;

        comm[11] = 0x01;

        comm[12] = 0x00; comm[13] = 0xFF; comm[14] = 0xFF; comm[15] = 0x00;

        hasWrite = Serial->write(comm,16);

        Serial->waitForBytesWritten(10);

        break;

    case 4:

        comm.resize(16);

        comm[0] = 0xAA; comm[1] = 0x55; comm[2] = 0x7E; comm[3] = 0x01;

        comm[4] = 0x00; comm[5] = 0x03;

        comm[6] = 0x11; comm[7] = 0x11; comm[8] = 0x11; comm[9] = 0x11;

        comm[10] = 0x00;

        comm[11] = 0x00;

        comm[12] = 0x00; comm[13] = 0xFF; comm[14] = 0xFF; comm[15] = 0x00;

        hasWrite = Serial->write(comm,16);

        Serial->waitForBytesWritten(10);

        break;
    default:
        break;
    }

    return hasWrite;
}



int WzSerialPort::WriteCommand(char commOdr,int *dataBuf, int dataLen,int Channel)
{
    int hasWrite = 0;

    QByteArray comm;
    int indx = 0;

//    for(int x = 0; x<dataLen;x++)
//    {
//        qDebug()<<"dataBuf["<<x<<"] = "<<dataBuf[x]<<endl;
//    }

    switch (commOdr)
    {

    case 4:

        comm.resize(17 + 2 * dataLen);

//        qDebug()<<"comm.size = " << comm.size();

        comm[0] = (BYTE)0xAA; comm[1] = 0x55; comm[2] = 0x7E; comm[3] = 0x01;

        comm[4] = 0x00; comm[5] = 0x04;

        comm[6] = (BYTE)((dataLen >> 24) & 0xFF);
        comm[7] = (BYTE)((dataLen >> 16) & 0xFF);
        comm[8] = (BYTE)((dataLen >> 8) & 0xFF);
        comm[9] = (BYTE)(dataLen & 0xFF);

        comm[10] = (BYTE)((Channel) & 0xFF);


        for (int i = 0; i < dataLen; i++)
        {
            comm[11 + indx] = (BYTE)(((dataBuf[i]) >> 8) & 0xFF);
            comm[11 + indx + 1] = (BYTE)((dataBuf[i]) & 0xFF);
            indx += 2;
        }
        comm[17 + 2 * dataLen - 4 ] = 0x00; comm[17 + 2 * dataLen - 3] = 0xFF; comm[17 + 2 * dataLen - 2] = 0xFF; comm[17 + 2 * dataLen - 1] = 0x00;

//        for(int x = 0;x < comm.size();x++)
//        {
//            qDebug()<<"comm["<< x <<"]= "<< QString::number(comm[x],16);
//        }


        hasWrite = Serial->write(comm, 17 + 2 * dataLen);

        Serial->waitForBytesWritten(10);

        break;
    default:

        break;
    }

    return hasWrite;
}

bool WzSerialPort::isSendCommandSuccessful(char CommanOdr)
{

    Serial->waitForReadyRead(10);

    int success_2[16] = {0xAA,0x55,0x7E,0x01,0x00,0x02,0x00,0x00,0x00,0x02,0xC8,0x01,0x00,0xFF,0xFF,0x00 };

    int success_4[16] = {0xAA,0x55,0x7E,0x01,0x00,0x04,0x00,0x00,0x00,0x02,0x00,0x01,0x00,0xFF,0xFF,0x00 };

    bool isSuccess = true;
    switch (CommanOdr)
    {
    case 2:
        for (int i = 0; i < 16; i++)
        {
            if ((BYTE)Recv[i]!= (BYTE)success_2[i]) {
                isSuccess = false;
                break;
            }
        }
        break;
    case 4:
        for (int i = 0; i < 16; i++)
        {
            if ((BYTE)Recv[i] != (BYTE)success_4[i]) {
                isSuccess = false;
                break;
            }
        }
        break;
    default:
        break;
    }

    Recv.clear();
    return isSuccess;
}


