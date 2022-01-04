#include "waveform.h"
#include "ui_waveform.h"
#include <QDebug>
#include <QMessageBox>
WaveForm::WaveForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaveForm)
{
    ui->setupUi(this);
    this->setFixedSize(1260,666);

    ui->lineGraphicsView_1->setRenderHint(QPainter::Antialiasing);
    ui->lineGraphicsView_2->setRenderHint(QPainter::Antialiasing);
    ui->lineGraphicsView_3->setRenderHint(QPainter::Antialiasing);
    WidgetInit();

    this->setWindowTitle("WaveForm");
}

WaveForm::~WaveForm()
{
    qDebug() << "WaveForm deconstruct start" << endl;
    delete ui;
}


void WaveForm::paramInit(const Config *cfig)
{
    peakNum = cfig->m_demodulation->m_peakNum;
    ValueMax = cfig->m_DataProcess->m_ValueMax;
    ValueMin = cfig->m_DataProcess->m_ValueMin;
    waveLength = cfig->m_DataProcess->m_WaveFormLength;

    qDebug()<<"WaveForm Min = " << ValueMin;
    qDebug()<<"WaveForm Max = " << ValueMax;

    if(cfig->m_demodulation->m_CacPhase == std::string("true"))
        isCalculatePhase = true;
    else
        isCalculatePhase = false;
}


void loadData()
{

}

void WaveForm::WidgetInit()
{
    //line 1 Init
    lineChart_1 = new QChart;
    lineSeries_1 = new QLineSeries();
    //lineSeries_1->setUseOpenGL(true);
    lineChart_1->addSeries(lineSeries_1);
    lineChart_1->legend()->hide();
    axisX_Line_1 = new QValueAxis();
    axisX_Line_1->setLabelsVisible(true);
    axisX_Line_1->setGridLineColor(Qt::gray);
    QPen Xpen;
    Xpen.setWidth(1);
    axisX_Line_1->setRange(0,waveLength);
    axisX_Line_1->setGridLinePen(Xpen);
    axisX_Line_1->setGridLineVisible(false);
    axisX_Line_1->setTickCount(20);
    axisX_Line_1->setMinorTickCount(5);
    axisX_Line_1->setMinorGridLineVisible(false);
    axisX_Line_1->hide();

    axisY_Line_1 = new QValueAxis();
    axisY_Line_1->setRange(ValueMin,ValueMax);
    axisY_Line_1->setTickCount(5);
    axisY_Line_1->setMinorTickCount(2);
    axisY_Line_1->setGridLineVisible(false);
    axisY_Line_1->setMinorGridLineVisible(false);

    lineChart_1->setAxisX(axisX_Line_1,lineSeries_1);
    lineChart_1->setAxisY(axisY_Line_1,lineSeries_1);
    ui->lineGraphicsView_1->setRenderHint(QPainter::Antialiasing);
    ui->lineGraphicsView_1->setChart(lineChart_1);
    ui->lineGraphicsView_1->setRubberBand(QChartView::RectangleRubberBand);

    lineChart_2 = new QChart;
    lineSeries_2 = new QLineSeries();
    //lineSeries_2->setUseOpenGL(true);

    lineChart_2->addSeries(lineSeries_2);
    lineChart_2->legend()->hide();
    axisX_Line_2 = new QValueAxis;
    axisX_Line_2->setRange(0,waveLength);
    axisX_Line_2->setLabelsVisible(true);
    axisX_Line_2->setGridLineColor(Qt::gray);
    axisX_Line_2->setGridLinePen(Xpen);
    axisX_Line_2->setGridLineVisible(false);
    axisX_Line_2->setTickCount(20);
    axisX_Line_2->setMinorTickCount(5);
    axisX_Line_2->setMinorGridLineVisible(false);
    axisX_Line_2->hide();

    axisY_Line_2 = new QValueAxis;
    axisY_Line_2->setTickCount(5);
    axisY_Line_2->setRange(ValueMin,ValueMax);
    axisY_Line_2->setMinorTickCount(2);
    axisY_Line_2->setGridLineVisible(false);
    axisY_Line_2->setMinorGridLineVisible(false);

    lineChart_2->setAxisX(axisX_Line_2,lineSeries_2);
    lineChart_2->setAxisY(axisY_Line_2,lineSeries_2);
    ui->lineGraphicsView_2->setRenderHint(QPainter::Antialiasing);
    ui->lineGraphicsView_2->setChart(lineChart_2);
    ui->lineGraphicsView_2->setRubberBand(QChartView::RectangleRubberBand);


    lineChart_3 = new QChart;
    lineSeries_3 = new QLineSeries();
    //lineSeries_3->setUseOpenGL(true);

    lineChart_3->addSeries(lineSeries_3);
    lineChart_3->legend()->hide();


    axisX_Line_3 = new QValueAxis();
    axisX_Line_3->setRange(0,waveLength);
    axisX_Line_3->setLabelsVisible(true);
    axisX_Line_3->setGridLineColor(Qt::gray);
    axisX_Line_3->setGridLinePen(Xpen);
    axisX_Line_3->setGridLineVisible(false);
    axisX_Line_3->setTickCount(20);
    axisX_Line_3->setMinorTickCount(5);
    axisX_Line_3->setMinorGridLineVisible(false);
    axisX_Line_3->hide();

    axisY_Line_3 = new QValueAxis();
    axisY_Line_3->setTickCount(5);
    axisY_Line_3->setRange(ValueMin,ValueMax);
    axisY_Line_3->setMinorTickCount(2);
    axisY_Line_3->setGridLineVisible(false);
    axisY_Line_3->setMinorGridLineVisible(false);

    lineChart_3->setAxisX(axisX_Line_3,lineSeries_3);
    lineChart_3->setAxisY(axisY_Line_3,lineSeries_3);

    ui->lineGraphicsView_3->setRenderHint(QPainter::Antialiasing);
    ui->lineGraphicsView_3->setChart(lineChart_3);
    ui->lineGraphicsView_3->setRubberBand(QChartView::RectangleRubberBand);


    //loadExampleData(1);
}

void WaveForm::on_lineEdit_1_returnPressed()
{
    int FBG1 = ui->lineEdit_1->text().toInt();
    int FBG2 = ui->lineEdit_2->text().toInt();
    int FBG3 = ui->lineEdit_3->text().toInt();

    if(FBG1 > peakNum)
    {
        ui->lineEdit_1->setText(QString::number(peakNum - 1));
        FBG1 = peakNum -1;
    }
    if(FBG1 < 1)
    {
        ui->lineEdit_1->setText(QString::number(1));
        FBG1 = 1;
    }
    qDebug()<< " line1 Changed "<<FBG1;
    emit SelectFBGChanged(FBG1,FBG2,FBG3);

    QString con = QString("WaveFrom line1 count: ")+QString::number(lineSeries_1->pointsVector().count());
    QMessageBox::information(NULL, "Title", con);
}

void WaveForm::on_lineEdit_2_returnPressed()
{
    int FBG1 = ui->lineEdit_1->text().toInt();
    int FBG2 = ui->lineEdit_2->text().toInt();
    int FBG3 = ui->lineEdit_3->text().toInt();
    if(FBG2 > peakNum)
    {
        ui->lineEdit_2->setText(QString::number(peakNum - 1));
        FBG2 = peakNum -1;
    }
    if(FBG2 < 1)
    {
        ui->lineEdit_2->setText(QString::number(1));
        FBG2 = 1;
    }

    qDebug()<< " line2 Changed "<<FBG2;
    emit SelectFBGChanged(FBG1,FBG2,FBG3);
    QString con = QString("WaveFrom line2 count: ")+QString::number(lineSeries_2->pointsVector().count());
    QMessageBox::information(NULL, "Title", con);
}

void WaveForm::on_lineEdit_3_returnPressed()
{
    int FBG1 = ui->lineEdit_1->text().toInt();
    int FBG2 = ui->lineEdit_2->text().toInt();
    int FBG3 = ui->lineEdit_3->text().toInt();

    if(FBG3 > peakNum)
    {
        ui->lineEdit_3->setText(QString::number(peakNum - 1));
        FBG3 = peakNum -1;
    }
    if(FBG3 < 1)
    {
        ui->lineEdit_3->setText(QString::number(1));
        FBG3 = 1;
    }

    qDebug()<< " line3 Changed "<<FBG3;
    emit SelectFBGChanged(FBG1,FBG2,FBG3);
    QString con = QString("WaveFrom line3 count: ")+QString::number(lineSeries_3->pointsVector().count());
    QMessageBox::information(NULL, "Title", con);
}

void WaveForm::loadExampleData(int index)
{
    QVector<QPointF> pointsSeries;

    for(int i = 1; i<=WAVE_WIDGET_LENGTH; i++){
        pointsSeries.append(QPointF(i,(qrand()%(10))));
    }
    qDebug()<<"pointsSeries Length=" <<pointsSeries.count();
    lineSeries_1->replace(pointsSeries);
    qDebug()<<"lineSeries_1 Length=" <<lineSeries_1->pointsVector().count();


    QVector<QPointF> pointsSeries2;

    for(int i =1; i<=WAVE_WIDGET_LENGTH; i++){
        pointsSeries2.append(QPointF(i,(qrand()%(6))));
    }

    qDebug()<<"pointsSeries Length=" <<pointsSeries2.count();
    lineSeries_2->replace(pointsSeries2);
    qDebug()<<"lineSeries_2 Length=" <<lineSeries_2->pointsVector().count();

    QVector<QPointF> pointsSeries3;

    for(int i = 1; i<=WAVE_WIDGET_LENGTH; i++){
        pointsSeries3.append(QPointF(i,(qrand()%(2))));
    }

    qDebug()<<"pointsSeries Length=" <<pointsSeries3.count();
    lineSeries_3->replace(pointsSeries3);
    qDebug()<<"lineSeries_3 Length=" <<lineSeries_3->pointsVector().count();
}

void WaveForm::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(plotWidget == NULL)
    {
        plotWidget = new WavePlot();
        plotWidget->setParam(waveLength,ValueMin,ValueMax);
        plotWidget->WidgetInit();
    }
    int ChartIndex = 0;

    if(event->button() == Qt::LeftButton)
    {
         QPoint curPos = mapFromGlobal(QCursor::pos());
         if( curPos.y()> 5 && curPos.y() < 215)
             ChartIndex = 1;
         else if(curPos.y()> 225 && curPos.y() < 435)
             ChartIndex = 2;
         else if(curPos.y()> 445 && curPos.y() < 655)
             ChartIndex = 3;
    }
    QVector<QPointF> points;
    switch (ChartIndex) {
    case 1:
        //if(isCalculatePhase == false)
        {
            points = lineSeries_1->pointsVector();
            plotWidget->loadWaveData(points);
            plotWidget->show();
            plotWidget->setWindowTitle(QString("#") + ui->lineEdit_1->text() + QString("WaveForm"));
        }
        break;
    case 2:
        points = lineSeries_2->pointsVector();
        plotWidget->loadWaveData(points);
        plotWidget->show();
        plotWidget->setWindowTitle(QString("#") + ui->lineEdit_2->text() + QString("WaveForm"));
        break;
    case 3:
        points = lineSeries_3->pointsVector();
        plotWidget->loadWaveData(points);
        plotWidget->show();
        plotWidget->setWindowTitle(QString("#") + ui->lineEdit_3->text() + QString("WaveForm"));
    default:
        break;
    }
}
