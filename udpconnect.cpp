#include "udpconnect.h"
#include "demodulation.h"
#include <QThread>
#include <QVector>
#include <QMessageBox>
#include <QMutex>

UDPConnect::UDPConnect(Config *cfig)
{
    AddressIP.setAddress(cfig->m_demodulation->IP.c_str());
    this->port = cfig->m_demodulation->Port;

    this->PeakNum = cfig->m_demodulation->m_peakNum;
    this->frequency=cfig->m_FPGACard->m_freq;
    saveFolder = cfig->m_DataProcess->m_path;
    isSave = cfig->m_DataProcess->m_isSave;
    isSend = cfig->m_DataProcess->m_isSend;
    waveLength = cfig->m_DataProcess->m_WaveFormLength;
    hasSend = 0;
    pFile= new FILE *[PeakNum];
    path = new QString[PeakNum];
    if(cfig->m_demodulation->m_CacPhase == std::string("false"))
    {
        DataType = SEND_ORIGNAL_DATA;
    }
    else
    {
        DataType = SEND_PHASE_DATA;
    }




    qDebug()<<"UDP Address : "<< AddressIP.toString()<< endl;
    qDebug()<<"UDP Port:" << port <<endl;
    qDebug()<<"UDP PeakNum" << PeakNum <<endl;

}

UDPConnect::~UDPConnect()
{
    qDebug() << "UDP deconstruct success" << endl;
    delete[] QueData;
    //delete[] data;
}

int UDPConnect::sendData(CirQueue<float>* que,char dataType)
{

    if(socket == NULL)
    {
        socket = new QUdpSocket();
    }

    float *dat;
    int hasWrite = 0;
    int num = 0;

    if(dataType == SEND_ORIGNAL_DATA)
    {
        dat = new float[PeakNum*3]();
        QByteArray data1((PeakNum*3+8)*SENDSIZE,(char)0);
        for(int i = 0; i < SENDSIZE; i++)
        {
            for(int j=0;j< PeakNum*3 ;j++)
            {
                dat[j] = que->pop();
                num = (PeakNum*3) * i + j;
                QueData[num] = dat[j];
            }
            Head[4] = dataType;
            Head[5] = (char)i;
            Head[6] = (char)0xFF;
            Head[7] = (char)0xFF;
            data1.replace(i*(PeakNum*3 + 8),8,Head,8);
            data1.replace(i*(PeakNum + 8) + 8,PeakNum * 3 * 4,(char*)dat,PeakNum * 3 * 4);
        }
        if(isSend)
            hasWrite = socket->writeDatagram(data1,AddressIP,port);
        //qDebug()<<"SEND_ORIGNAL_DATA Has Write" << hasWrite <<"bits to UDP"<<endl;
    }
    else
    {
        dat = new float[PeakNum]();
        QByteArray data2((PeakNum+8)*SENDSIZE,(char)0);
        //int x = 0;
        for(int i = 0; i < SENDSIZE; i++)
        {
            for(int j=0;j < PeakNum;j++)
            {
                dat[j] = que->pop();
                num = PeakNum * i + j;
                QueData[num] = dat[j];
                //x++;
            }
            Head[4] = dataType;
            Head[5] = (char)i;
            Head[6] = (char)0xFF;
            Head[7] = (char)0xFF;
            data2.replace(i*(PeakNum + 8),8,Head,8);
            data2.replace(i*(PeakNum + 8) + 8,PeakNum *4,(char*)dat,PeakNum *4);

        }
        //qDebug()<<"hasSend:"<< x ;
        if(isSend)
            hasWrite = socket->writeDatagram(data2,AddressIP,port);

        //qDebug()<<"SEND_other_DATA Has Write" << hasWrite <<"bits to UDP"<<endl;
    }
    //qDebug()<<"After send Queue size = "<<que.size();


    delete[] dat;
    return hasWrite;
}
void UDPConnect::executeData()
{
    qDebug()<<"slot begin execute";
}

void UDPConnect::executeSendData(CirQueue<float>* que, char Type)
{
    //qDebug() <<"UDP begin send data";
    //qDebug() <<"Current Thread:"<< QThread::currentThread();

    if(QueData == NULL)
    {

        int Len = 0;
        if(Type == SEND_ORIGNAL_DATA)
        {
            Len = PeakNum*3*SENDSIZE;
            QueData = new float[Len]();

        }
        else
        {
            Len = PeakNum * SENDSIZE;
            QueData = new float[Len]();
        }

        qDebug()<<"QueData length = "<< Len;
    }

    int hasWrite = sendData(que,Type);
    if(is_saveData)
    {
        //qDebug()<<"save data2Bin()  181" <<endl;
        saveData2Bin(QueData);


    }

    //loadSeriesData(Type);调用有时程序会未响应
    hasSend+= hasWrite;
}

void UDPConnect::initSeriesParam(
        QLineSeries *line1,
        QLineSeries *line2,
        QLineSeries *line3)
{
    m_lineSeries_1 = line1;
    m_lineSeries_2 = line2;
    m_lineSeries_3 = line3;
}

void UDPConnect::changeSelectFBG(int FBG1, int FBG2, int FBG3)
{
    selectFBG_1 = FBG1;
    selectFBG_2 = FBG2;
    selectFBG_3 = FBG3;
}

void UDPConnect::loadSeriesData(char dataType)
{
    qint64 range = waveLength;
    //第一条线

    float *dat = new float[SENDSIZE]();

    {
        if (dataType == SEND_ORIGNAL_DATA)
        {
            for(int x = 0; x <SENDSIZE; x++)
            {
                dat[x] = QueData[PeakNum*3*x + selectFBG_1 + 0];
            }
        }
        else{
            for(int x = 0; x <SENDSIZE; x++)
            {
                dat[x] = QueData[PeakNum*x + selectFBG_1];
            }
        }
        QVector<QPointF> oldPoints1 = m_lineSeries_1->pointsVector();
        QVector<QPointF> points1;
        //qDebug()<<"m_lineSeries1 size = " << m_lineSeries_1->pointsVector().count();

        if (oldPoints1.count() < range) {
           points1 = oldPoints1;
        } else
        {
            for (int i = SENDSIZE; i < oldPoints1.count(); i++)
                points1.append(QPointF(i - SENDSIZE, oldPoints1.at(i).y()));
        }

        qint64 size = points1.count();
        for (int k = 0; k < SENDSIZE; k++)
            points1.append(QPointF(k + size, dat[k]));

        m_lineSeries_1->replace(points1);
    }
    //第二条线
    {
        if (dataType == SEND_ORIGNAL_DATA)
        {
            for(int x = 0; x <SENDSIZE; x++)
            {
                dat[x] = QueData[PeakNum*3*x + selectFBG_1 + 1];
            }
        }
        else{
            for(int x = 0; x <SENDSIZE; x++)
            {
                dat[x] = QueData[PeakNum*x + selectFBG_2];
            }
        }
        QVector<QPointF> oldPoints2 = m_lineSeries_2->pointsVector();
        QVector<QPointF> points2;
        //qDebug()<<"m_lineSeries2 size = " << m_lineSeries_2->pointsVector().count();

        if (oldPoints2.count() < range) {
            points2 = m_lineSeries_2->pointsVector();
        } else {
            for (int i = SENDSIZE; i < oldPoints2.count(); i++)
                points2.append(QPointF(i - SENDSIZE, oldPoints2.at(i).y()));
        }

        qint64 size = points2.count();
        for (int k = 0; k < SENDSIZE; k++)
            points2.append(QPointF(k + size, dat[k]));

        m_lineSeries_2->replace(points2);
    }
    //第三条线
    {
        if (dataType == SEND_ORIGNAL_DATA)
        {
            for(int x = 0; x <SENDSIZE; x++)
            {
                dat[x] = QueData[PeakNum*3*x + selectFBG_1 + 2];
            }
        }
        else{
            for(int x = 0; x <SENDSIZE; x++)
            {
                dat[x] = QueData[PeakNum*x + selectFBG_3];
            }
        }
        QVector<QPointF> oldPoints3 = m_lineSeries_3->pointsVector();
        QVector<QPointF> points3;

        if (oldPoints3.count() < range) {
            points3 = m_lineSeries_1->pointsVector();
        } else {
            for (int i = SENDSIZE; i < oldPoints3.count(); i++)
                points3.append(QPointF(i - SENDSIZE, oldPoints3.at(i).y()));
        }

        qint64 size = points3.count();
        for (int k = 0; k < SENDSIZE; k++)
            points3.append(QPointF(k + size, dat[k]));

        m_lineSeries_3->replace(points3);
    }

    delete[] dat;
}

void UDPConnect::saveData2Bin(float *data)//写入文件重写为ofstream
{
    QMutexLocker locker1(&writeLock);
    if(DataType == SEND_ORIGNAL_DATA)
    {
        int Len = PeakNum * SENDSIZE;
        float *CH1 = new float[Len]();
        float *CH2 = new float[Len]();
        float *CH3 = new float[Len]();

        for(size_t i = 0; i < PeakNum * SENDSIZE; i++)
        {
            CH1[i] = data[i*3 + 0];
            CH2[i] = data[i*3 + 1];
            CH3[i] = data[i*3 + 2];
        }
        fwrite(CH1,sizeof(float),PeakNum * SENDSIZE,pFile[0]);
        //qDebug() << "CH1[0] = " << CH1[0] << endl;
        fwrite(CH2,sizeof(float),PeakNum * SENDSIZE,pFile[1]);
        fwrite(CH3,sizeof(float),PeakNum * SENDSIZE,pFile[2]);

        delete[] CH1;
        delete[] CH2;
        delete[] CH3;

    }
    else
    {//存储区域2、4、6、8、10、12、14、16、18、20的数据（根据寻峰的改变可能会变成1、3、5.....)

       float (*area)[SENDSIZE];
        area=new float[PeakNum][SENDSIZE]();
        for(size_t i=0;i<PeakNum;i++)
        {
            for(size_t n=0;n<SENDSIZE;n++)
            {
              area[i][n]=data[i+PeakNum*n];
            }

        }
        for(int i=0;i<PeakNum;i++)
        {

            fwrite(area[i],sizeof(float),SENDSIZE,pFile[i]);
        }



        delete []area;
    }
}

void UDPConnect::changeFileName()
{
    QMutexLocker locker1(&writeLock);
    if(DataType == SEND_ORIGNAL_DATA)
    {
        fclose(pFile[0]);
        fclose(pFile[1]);
        fclose(pFile[2]);

        systemDate = QDateTime::currentDateTime();
        systemTime = QTime::currentTime();
        QString path1 = QString(saveFolder.c_str())+QString("/[CH1][")+QString::number(PeakNum)
                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");
        QString path2 = QString(saveFolder.c_str())+QString("/[CH2][")+QString::number(PeakNum)
                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");
        QString path3 = QString(saveFolder.c_str())+QString("/[CH3][")+QString::number(PeakNum)
                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");

        //qDebug()<<"file Name:" << path1;

        //QMessageBox::information(NULL,"Fileapth",path1.toStdString().c_str());

        pFile[0] = fopen(path1.toStdString().c_str(),"ab+");
        pFile[1] = fopen(path2.toStdString().c_str(),"ab+");
        pFile[2] = fopen(path3.toStdString().c_str(),"ab+");
    }
    else
    {
        systemDate = QDateTime::currentDateTime();
        systemTime = QTime::currentTime();
        for(int i=0;i<PeakNum;i++)
        {
            fclose(pFile[i]);
            path[i] =QString(saveFolder.c_str())+QString("/[")+QString::number(i)+QString("]")+
            systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss")+QString("_")+QString::number((int)(frequency/1000))+QString("KHz.bin");
        }
        for(int i=0;i<PeakNum;i++)
        {
            pFile[i]=fopen(path[i].toStdString().c_str(),"ab+");
        }
    }
}
