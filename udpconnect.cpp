#include "udpconnect.h"
#include "demodulation.h"
#include <QThread>
#include <QVector>
#include <QMessageBox>
#include <QMutex>

UDPConnect::UDPConnect(Config *cfig, int& sendsize):
    SENDSIZE(sendsize),
    hasNextFilename(false)
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
    path = new QString[PeakNum];
    filesize = (int)(frequency / SENDSIZE) * 60;
    outfile = make_shared<vector<ofstream>>();

    if(cfig->m_demodulation->m_CacPhase == std::string("false"))
    {
        DataType = SEND_ORIGNAL_DATA;
    }
    else
    {
        DataType = SEND_PHASE_DATA;
    }

    connect(this, SIGNAL(sendNextFile()), this, SLOT(changeFileName()));


    qDebug()<<"UDP Address : "<< AddressIP.toString()<< endl;
    qDebug()<<"UDP Port:" << port <<endl;
    qDebug()<<"UDP PeakNum" << PeakNum <<endl;

}

UDPConnect::~UDPConnect()
{
    qDebug() << "UDP deconstruct success" << endl;
    delete[] path;
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
        //cout << "outfile.size" << outfile->size() << endl;
        bool emptyQueue = true;
        saveData2Bin(QueData, emptyQueue);


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
/*使用字节流存储*/
void UDPConnect::saveData2Bin(float *data, bool &now)
{
    QMutexLocker locker1(&writeLock);

    if(DataType == SEND_ORIGNAL_DATA)
    {/* 存储原始3通道数据*/

        for(size_t i = 0; i < 3 * PeakNum * SENDSIZE; ++i)
        {

            const float& tmp = data[i];
            switch (i % 3)
            {
            case 0:
                outfile1.write((const char*) &tmp, sizeof(float));
                break;
            case 1:
                outfile2.write((const char*) &tmp, sizeof(float));
                break;
            case 2:
                outfile3.write((const char*) &tmp, sizeof(float));
                break;
            default:
                break;
            }

        }
        if(outfile1.tellp() >= frequency * 60 * 4 * PeakNum && hasNextFilename)
        {
            locker1.unlock();
            emit sendNextFile();
        }
    }
    else
    {/* 存储解调数据*/
        cout << "outfile.size" << outfile->size() << endl;
        auto itn = outfile->begin();
        for(size_t i = 0; i < PeakNum * SENDSIZE; ++i)
        {
            const float &tmp = data[i];

            if(itn != outfile->cend())
            {
                itn->write((const char*) &tmp, sizeof(float));
                //qDebug() << "pos = "  << itn->tellp() << endl;
                ++itn;

            }
            else
            {
                itn = outfile->begin();
                itn->write((const char*) &tmp, sizeof(float));
                ++itn;
            }
        }
       itn = outfile->begin();
       if(itn->tellp() >= frequency * 60 * 4 && hasNextFilename)
       {
           locker1.unlock();
           emit sendNextFile();
       }

    }

}

/*旧版存储函数，已经弃用*/
void UDPConnect::saveData2Bin(float *data)
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
    {
        float **area = new float*[PeakNum];
        for(size_t i = 0; i < PeakNum; ++i)
        {
            area[i] = new float[SENDSIZE];
        }
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

/* 初次生成存储文件名*/
void UDPConnect::changeFileNameOnce(QDateTime &systemDate, QTime &systemTime)
{
//    if(DataType == SEND_ORIGNAL_DATA)
//    {

//        QString path1 = QString(saveFolder.c_str())+QString("/[CH1][")+QString::number(PeakNum)
//                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");
//        QString path2 = QString(saveFolder.c_str())+QString("/[CH2][")+QString::number(PeakNum)
//                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");
//        QString path3 = QString(saveFolder.c_str())+QString("/[CH3][")+QString::number(PeakNum)
//                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");

//        pFile[0] = fopen(path1.toStdString().c_str(),"ab+");
//        pFile[1] = fopen(path2.toStdString().c_str(),"ab+");
//        pFile[2] = fopen(path3.toStdString().c_str(),"ab+");
//    }
//    else
//    {
//        for(int i=0;i<PeakNum;i++)
//        {
//             path[i] =QString(saveFolder.c_str())+QString("/[")+QString::number(i)+QString("]")+
//             systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss")+QString("_")+QString::number((int)(frequency/1000))+QString("KHz.bin");
//        }

//        for(int i=0;i<PeakNum;i++)
//        {
//             pFile[i]=fopen(path[i].toStdString().c_str(),"ab+");
//        }
//    }
    if(DataType == SEND_ORIGNAL_DATA)
    {

        saveFilename1 = QString(saveFolder.c_str())+QString("/[CH1][")+QString::number(PeakNum)
                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");
        saveFilename2 = QString(saveFolder.c_str())+QString("/[CH2][")+QString::number(PeakNum)
                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");
        saveFilename3 = QString(saveFolder.c_str())+QString("/[CH3][")+QString::number(PeakNum)
                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");

        outfile1 = ofstream(saveFilename1.toStdString().data(), ofstream::binary);
        outfile2 = ofstream(saveFilename2.toStdString().data(), ofstream::binary);
        outfile3 = ofstream(saveFilename3.toStdString().data(), ofstream::binary);

    }
    else
    {
        //systemDate = QDateTime::currentDateTime();
        //systemTime = QTime::currentTime();
        for(int i=0;i<PeakNum;i++)
        {
//            fclose(pFile[i]);
            path[i] =QString(saveFolder.c_str())+QString("/[")+QString::number(i)+QString("]")+
            systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss")+QString("_")+QString::number((int)(frequency/1000))+QString("KHz.bin");
        }
        for(int i=0;i<PeakNum;i++)
        {
//            pFile[i]=fopen(path[i].toStdString().c_str(),"ab+");
            ofstream of(path[i].toStdString().data(), ofstream::binary);
            outfile->push_back(std::move(of));//ofstream没有拷贝构造函数，因此只能用移动构造函数
            qDebug() << "outfilesize 442:" << outfile->size();
        }

    }
}

void UDPConnect::changeFileName()
{

    QMutexLocker locker1(&writeLock);
    if(DataType == SEND_ORIGNAL_DATA)
    {
        outfile1.close();
        outfile2.close();
        outfile3.close();
//        fclose(pFile[0]);
//        fclose(pFile[1]);
//        fclose(pFile[2]);

//        systemDate = QDateTime::currentDateTime();
//        systemTime = QTime::currentTime();
//        saveFilename1 = QString(saveFolder.c_str())+QString("/[CH1][")+QString::number(PeakNum)
//                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");
//        saveFilename2 = QString(saveFolder.c_str())+QString("/[CH2][")+QString::number(PeakNum)
//                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");
//        saveFilename3 = QString(saveFolder.c_str())+QString("/[CH3][")+QString::number(PeakNum)
//                +QString("]")+systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss") + QString(".bin");

        outfile1 = ofstream(saveFilename1.toStdString().data(), ofstream::binary);
        outfile2 = ofstream(saveFilename2.toStdString().data(), ofstream::binary);
        outfile3 = ofstream(saveFilename3.toStdString().data(), ofstream::binary);
//        pFile[0] = fopen(saveFilename1.toStdString().c_str(),"ab+");
//        pFile[1] = fopen(saveFilename2.toStdString().c_str(),"ab+");
//        pFile[2] = fopen(saveFilename3.toStdString().c_str(),"ab+");
    }
    else
    {
        if(outfile->size() > 0)
        {
            for(auto& outf : *outfile) outf.close();
            outfile->clear();
        }

//        systemDate = QDateTime::currentDateTime();
//        systemTime = QTime::currentTime();
//        for(int i=0;i<PeakNum;i++)
//        {
//            fclose(pFile[i]);
//            path[i] =QString(saveFolder.c_str())+QString("/[")+QString::number(i)+QString("]")+
//            systemDate.toString("yyyyMMdd")+systemTime.toString("hhmmss")+QString("_")+QString::number((int)(frequency/1000))+QString("KHz.bin");
//        }
        for(int i=0;i<PeakNum;i++)
        {
//            pFile[i]=fopen(path[i].toStdString().c_str(),"ab+");
            ofstream of(path[i].toStdString().data(), ofstream::binary);
            outfile->push_back(std::move(of));//ofstream没有拷贝构造函数，因此只能用移动构造函数
        }

    }
    hasNextFilename = false;//重置文件名标志
    locker1.unlock();
}

/*产生下一个文件的文件名*/
void UDPConnect::getFilename()
{
    QDateTime nextDate = QDateTime::currentDateTime();
    QTime nextTime = QTime::currentTime();
    if(DataType == SEND_ORIGNAL_DATA)
    {
        saveFilename1 = QString(saveFolder.c_str())+QString("/[CH1][")+QString::number(PeakNum)
                +QString("]")+nextDate.toString("yyyyMMdd")+nextTime.toString("hhmmss") + QString(".bin");
        saveFilename2 = QString(saveFolder.c_str())+QString("/[CH2][")+QString::number(PeakNum)
                +QString("]")+nextDate.toString("yyyyMMdd")+nextTime.toString("hhmmss") + QString(".bin");
        saveFilename3 = QString(saveFolder.c_str())+QString("/[CH3][")+QString::number(PeakNum)
                +QString("]")+nextDate.toString("yyyyMMdd")+nextTime.toString("hhmmss") + QString(".bin");
    }
    else
    {
        for(int i = 0; i < PeakNum; ++i)
        {
            path[i] =QString(saveFolder.c_str())+QString("/[")+QString::number(i)+QString("]")+
            nextDate.toString("yyyyMMdd")+nextTime.toString("hhmmss")+QString("_")+QString::number((int)(frequency/1000))+QString("KHz.bin");
        }
     }
    hasNextFilename = true;//已经过了1分钟，获取了下一分钟的文件名
}

//int& UDPConnect::setSENDSIZE(int &sendsize)
//{
//    SENDSIZE = sendsize;
//    return SENDSIZE;
//}
