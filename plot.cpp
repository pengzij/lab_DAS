#include "plot.h"
#include "ui_plot.h"
#include <QPen>
#include<QPoint>
Plot::Plot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Plot)
{
    ui->setupUi(this);
    this->setWindowTitle("PeakForm");
    //this->setAttribute(Qt::WA_DeleteOnClose,true);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing); //抗锯齿

    m_chart = new QChart;



    m_series = new QLineSeries;
    m_chart->addSeries(m_series);

    m_scatLine = new QScatterSeries;
    m_scatLine->setMarkerSize(8.0);
    m_scatLine->setColor(Qt::red);
    m_chart->addSeries(m_scatLine);

    //m_chart->setBackgroundBrush(QBrush(qRgb(50,50,50)));

    axisX = new QValueAxis;
    axisX->setLabelsVisible(true);
    axisX->setRange(0, 1000);
    axisX->setGridLineColor(Qt::gray);

    QPen Xpen;
    Xpen.setWidth(1);
    axisX->setGridLinePen(Xpen);
    axisX->setGridLineVisible(false);
    axisX->setTickCount(10);

    axisX->setMinorTickCount(5);
    axisX->setMinorGridLineVisible(false);
    axisY = new QValueAxis;
    axisY->setTickCount(10);
    axisY->setMinorTickCount(2);

    axisY->setRange(-1,200);
    axisY->setGridLineVisible(false);
    axisY->setMinorGridLineVisible(false);

    m_chart->setAxisX(axisX, m_series);
    m_chart->setAxisY(axisY, m_series);

    m_chart->setAxisX(axisX, m_scatLine);
    m_chart->setAxisY(axisY, m_scatLine);

    m_chart->legend()->hide();


    m_chart->setContentsMargins(0,0,0,0);
    m_chart->setMargins(QMargins(0,0,0,0));
    m_chart->setBackgroundRoundness(0);

    m_chart->setAutoFillBackground(true);


    connect(m_scatLine,&QScatterSeries::hovered,this,&Plot::slotPointHoverd);


    ui->ChannelSelectBox->setEnabled(true);


    ui->graphicsView->setChart(m_chart);
    ui->graphicsView->setRubberBand(QChartView::RectangleRubberBand);

    m_valueLable = new QLabel(this);

    readPeakData();

    YMax = 0;
    isSearchPeakAgain = false;

    loadData(CH1Pos,CH1PeakValue,CH1PeakNum,CH1DataAll,lenOfChannel);
}


Plot::~Plot()
{
    qDebug() << "Plot deconstruct success" << endl;
    FreeMemory();
    delete ui;
}

void Plot::loadData(int* peakPos, float* peakValue,unsigned int PeakNum, float *dataAll,unsigned int dataLen){
    QVector<QPointF> pointsSeries;
    QVector<QPointF> pointsScatLine;
    for(unsigned int i = 0; i<dataLen; i++){
        pointsSeries.append(QPointF(i,dataAll[i]));
    }
    for(unsigned int i = 0; i<PeakNum; i++)
    {
        pointsScatLine.append(QPointF(peakPos[i], peakValue[i]));
        if(peakValue[i] > YMax)
            YMax = peakValue[i];
    }
    QString Title = "PeakNum:" + QString::number(PeakNum);

    axisX->setRange(0,dataLen + 100);
    axisY->setRange(-1,YMax+100);
    m_series->replace(pointsSeries);
    m_scatLine->replace(pointsScatLine);

    m_chart->setTitle(Title);
    QFont font;
    font.setFamily(QString("Consolas"));
    font.setPointSize(15);
    m_chart->setTitleFont(font);
}

void Plot::loadData(int x){
    QVector<QPointF> pointsSeries;
    QVector<QPointF> pointsScatLine;
    for(int i = 0; i<10000; i++){
        pointsSeries.append(QPointF(i,(qrand()%(x*40))));
    }
    for(int i = 0; i<100; i++)
    {
        pointsScatLine.append(QPointF(i*20,qrand()%(x)));
    }

    m_series->replace(pointsSeries);
    m_scatLine->replace(pointsScatLine);
}

void Plot::slotPointHoverd(const QPointF &point, bool state)
{
    if(state)
    {

        QString pos = QString("(") + QString::number(point.x())
                +QString(",")+QString::number(point.y())+QString(")");
        m_valueLable->setText(pos);
        QPoint curPos = mapFromGlobal(QCursor::pos());
        m_valueLable->move(curPos.x(),curPos.y());
        m_valueLable->show();

    }else
    {
        m_valueLable->hide();
    }
}


void Plot::wheelEvent(QWheelEvent *event)
{
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    if(event->delta() > 0)
    {
        m_chart->zoomIn();
    }
    else
    {
        m_chart->zoomOut();
    }
}


void Plot::on_ChannelSelectBox_currentTextChanged(const QString &arg1)
{
    int channel = ui->ChannelSelectBox->currentText().toInt();
    if(isSearchPeakAgain)
    {
        readPeakData();
        isSearchPeakAgain = false;
    }
    //qDebug()<<"channel = " << channel << endl;
    switch(channel){
    case 1:
        loadData(CH1Pos,CH1PeakValue,CH1PeakNum,CH1DataAll,lenOfChannel);
        //loadData(channel);
        break;
    case 2:
        loadData(CH2Pos,CH2PeakValue,CH2PeakNum,CH2DataAll,lenOfChannel);
        //loadData(channel);
        break;
    case 3:
        loadData(CH3Pos,CH3PeakValue,CH3PeakNum,CH3DataAll,lenOfChannel);
       // loadData(channel);
        break;
    default:
        break;
    }
}

bool Plot::readPeakData(){
    bool readSuc = true;
    char* CH1fileName = (char*)"C:/DAS/peak1.bin";
    char* CH2fileName = (char*)"C:/DAS/peak2.bin";
    char* CH3fileName = (char*)"C:/DAS/peak3.bin";
    char  logStr[256] = { 0 };
    FILE *pFile1 = fopen(CH1fileName,"rb");

    if (pFile1 == NULL)
    {
        sprintf(logStr, "Open Read Peak File: %s Faied \n",CH1fileName);
        LOG(logStr, "C:/DAS/log.txt");
        readSuc = false;
    }
    else{
        fread(&CH1PeakNum,sizeof(unsigned int),1,pFile1);
        fread(&lenOfChannel,sizeof(unsigned int),1,pFile1);
        if(CH1Pos == NULL)
        {
            qDebug()<<"New Space for CH1Pos"<<endl;
            CH1Pos = new int[CH1PeakNum]();
        }
        fread(CH1Pos,sizeof(int),CH1PeakNum,pFile1);
        if(CH1PeakValue == NULL)
            CH1PeakValue = new float[CH1PeakNum]();
        fread(CH1PeakValue,sizeof(float),CH1PeakNum,pFile1);
        if(CH1DataAll == NULL)
            CH1DataAll = new float[lenOfChannel]();
        fread(CH1DataAll,sizeof(float),lenOfChannel,pFile1);

        qDebug()<<"readPeak1File succeed!";

        fclose(pFile1);
    }

    FILE *pFile2= fopen(CH2fileName,"rb");

    if (pFile2 == NULL)
    {
        sprintf(logStr, "Open Read Peak File: %s Faied \n",CH2fileName);
        LOG(logStr, "C:/DAS/log.txt");
        readSuc = false;
    }
    else{
        fread(&CH2PeakNum,sizeof(unsigned int),1,pFile2);
        fread(&lenOfChannel,sizeof(unsigned int),1,pFile2);
        if(CH2Pos == NULL)
            CH2Pos = new int[CH2PeakNum]();
        fread(CH2Pos,sizeof(int),CH2PeakNum,pFile2);
        if(CH2PeakValue == NULL)
            CH2PeakValue = new float[CH2PeakNum]();
        fread(CH2PeakValue,sizeof(float),CH2PeakNum,pFile2);
        if(CH2DataAll == NULL)
            CH2DataAll = new float[lenOfChannel]();
        fread(CH2DataAll,sizeof(float),lenOfChannel,pFile2);

        fclose(pFile2);

        qDebug()<<"readPeak2File succeed!";
    }

    FILE *pFile3 = fopen(CH3fileName,"rb");

    if (pFile3 == NULL)
    {
        sprintf(logStr, "Open Read Peak File: %s Faied\n",CH3fileName);
        LOG(logStr, "C:/DAS/log.txt");
        readSuc = false;
    }
    else{
        fread(&CH3PeakNum,sizeof(unsigned int),1,pFile3);
        fread(&lenOfChannel,sizeof(unsigned int),1,pFile3);
        if(CH3Pos == NULL)
            CH3Pos = new int[CH3PeakNum]();
        fread(CH3Pos,sizeof(int),CH3PeakNum,pFile3);
        if(CH3PeakValue == NULL)
            CH3PeakValue = new float[CH3PeakNum]();
        fread(CH3PeakValue,sizeof(float),CH3PeakNum,pFile3);
        if(CH3DataAll == NULL)
            CH3DataAll = new float[lenOfChannel]();
        fread(CH3DataAll,sizeof(float),lenOfChannel,pFile3);
        fclose(pFile3);

        qDebug()<<"readPeak3File succeed!";
    }
    return readSuc;
}

void Plot::on_ConfirmPeak_Btn_clicked()
{
    emit ConfirmSearchPeakSignal();
    this->ui->ResearchPeak_Btn->setEnabled(false);
    qDebug()<<"ConfirmPeak_Btn_clicked"<<endl;
}

void Plot::on_ResearchPeak_Btn_clicked()
{
    isSearchPeakAgain = true;
    emit ResearchPeakSignal();
    //qDebug()<<"ResearchPeak_Btn_clicked"<<endl;
}

void Plot::FreeMemory()
{
    delete[] CH1Pos;
    delete[] CH2Pos;
    delete[] CH3Pos;

    delete[] CH1DataAll;
    delete[] CH2DataAll;
    delete[] CH3DataAll;

    delete[] CH1PeakValue;
    delete[] CH2PeakValue;
    delete[] CH3PeakValue;

    delete m_chart;
    delete m_scatLine;
    delete m_series;
    delete axisX;
    delete axisY;
    delete m_valueLable;
 }
