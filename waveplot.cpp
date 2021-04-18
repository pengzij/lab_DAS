#include "waveplot.h"
#include "ui_waveplot.h"
#include <QMenu>
#include <QAction>
WavePlot::WavePlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WavePlot)
{
    ui->setupUi(this);
    ui->graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->graphicsView,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(showMenu(QPoint)));

}

WavePlot::~WavePlot()
{
    delete ui;
}


void WavePlot::WidgetInit()
{
    ui->graphicsView->setRenderHint(QPainter::Antialiasing); //抗锯齿
    m_chart = new QChart;
    m_series = new QLineSeries;
    m_chart->addSeries(m_series);
    m_chart->legend()->hide();

    axisX = new QValueAxis;
    axisX->setLabelsVisible(true);
    axisX->setRange(0, WaveLength);
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
    qDebug()<<"Min = " << ValueMin;
    qDebug()<<"Max = " << ValueMax;
    axisY->setRange(ValueMin,ValueMax);
    axisY->setGridLineVisible(false);
    axisY->setMinorGridLineVisible(false);

    m_chart->setAxisX(axisX, m_series);
    m_chart->setAxisY(axisY, m_series);

    ui->graphicsView->setChart(m_chart);
    ui->graphicsView->setRubberBand(QChartView::RectangleRubberBand);
}

void WavePlot::setParam(int wavelen, float min, float max)
{
    WaveLength = wavelen;
    this->ValueMax = max;
    this->ValueMin = min;

    qDebug()<<"WavePlot ValueMin = " << this->ValueMin;
    qDebug()<<"WavePlot ValueMax = " << this->ValueMax;
}

void WavePlot::loadWaveData(QVector<QPointF> &points)
{
    float max = 0;
    float min = 0;
    for(size_t i = 0; i < WaveLength; i++)
    {
        if(points[i].y() > max)
        {
            max = points[i].y();
        }
        if(points[i].y() < min)
        {
            min = points[i].y();
        }
    }
    this->ValueMax = max;
    this->ValueMin = min;
    axisY->setRange(ValueMin,ValueMax);
    m_series->replace(points);
}

void WavePlot::showMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu(ui->graphicsView);
    QAction *pMenu1 = new QAction("ReStore Axis",ui->graphicsView);
    QObject::connect(pMenu1,&QAction::triggered,this,&WavePlot::restoreAxis);

    menu->addAction(pMenu1);
    menu->move(cursor().pos());
    menu->show();
}

void WavePlot::restoreAxis()
{
    qDebug()<<"WavePlot ValueMin = " << this->ValueMin;
    qDebug()<<"WavePlot ValueMax = " << this->ValueMax;
    axisX->setRange(0,WaveLength);
    axisY->setRange(ValueMin,ValueMax);
}
