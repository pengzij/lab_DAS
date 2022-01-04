#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "demodulation.h"

#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtMath>
#include <QList>
#include <sys/time.h>

static int ii = 0;

MainWidget::MainWidget(const shared_ptr<GetConfig> gcfg, shared_ptr<CirQueue<float>> wavedataque, int tmInterval, int window_max_x, int xScale, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    chart(new QChart),
    timer2(new QTimer),
    WaveDataQue(wavedataque),
    TIMEINTERVAL(tmInterval),//定时器时间间隔
    FRE(gcfg->getConfig_freqency() / 1000),//频率
    WINDOW_MAX_X(window_max_x),//窗口x轴范围
    XSACLE(xScale),//x轴时间刻度
    tip(0),
    count(0),
    isStopping(false),
    isStop(false),
    wavecase(0),
    cur_x(0),
    last_queue_size(0),
    cur_queue_size(0),
    peakNum(gcfg->getPeakNum()),
    isUpdataing(false),
    clearWave(true),
    lastShowpeakNum(-1)//初始化为不可能的数据
{
    ui->setupUi(this);
    timer2->setInterval(TIMEINTERVAL);
    LEN_PER = FRE * TIMEINTERVAL * 1.34;
    SHOWDATA_MAX = WINDOW_MAX_X * 10;
    cur_len_per = LEN_PER;
}

MainWidget::~MainWidget()
{
    timer2->stop();
    Sleep(10);
    delete series;
    delete tip;
    delete chart;
    delete timer2;
    delete chartView;
    delete ui;
    qDebug() << "MainWidget deconstruct success" << endl;
}


void MainWidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) {
        chart->zoom(1.6);
    } else {
        chart->zoom(9.5/11);
    }

    QWidget::wheelEvent(event);
}

void MainWidget::initUI()
{
    qDebug() << "initUI peaknum = " << peakNum <<endl;
    QString intostring;
    for (int i = 1; i <= peakNum; i++)
    {
        ui->comboBox->addItem(intostring.setNum(i));
    }

    initChart();
    initSlot();
    if(!timer2->isActive())
        timer2->start();
}

void MainWidget::initChart()
{
    series = new QLineSeries;

    chart->addSeries(series);

//    series->setUseOpenGL(true);

    chart->createDefaultAxes();

    chart->axisY()->setRange(-3, 3);
    chart->axisX()->setRange(0, WINDOW_MAX_X);

    chart->axisX()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisY()->setTitleFont(QFont("Microsoft YaHei", 10, QFont::Normal, true));
    chart->axisX()->setTitleText("Time/ms");
    chart->axisY()->setTitleText("Amplitude/rad");

    chart->axisX()->setGridLineVisible(false);
    chart->axisY()->setGridLineVisible(false);

    /* hide legend */
    chart->legend()->hide();

    chartView = new ChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->mainHorLayout->addWidget(chartView);
}

void MainWidget::initSlot()
{
    connect(timer2, SIGNAL(timeout()), this, SLOT(flashwave()));
    connect(ui->stopBtn, SIGNAL(clicked(bool)), this, SLOT(buttonSlot()));
    connect(series, SIGNAL(hovered(QPointF, bool)), this, SLOT(tipSlot(QPointF,bool)));
}

int& MainWidget::setSENDSIZE(int &sendsize)
{
    SENDSIZE = sendsize;
    return SENDSIZE;
}

void MainWidget::updateData(CirQueue<float> *waveque, int peaknum)
{
    sendData(waveque);
    peakNum = peaknum;
    if(!timer2->isActive())
        timer2->start();
}

void MainWidget::sendData(CirQueue<float> *que)
{
    int cur_chosen_peak = ui->comboBox->currentIndex();

    for(int i = 0;i < SENDSIZE; i ++)
    {
        for(int j = 0; j < peakNum; j++)
        {
            if(j == cur_chosen_peak)
            {
                float dat = que->pop();
                //showData.push_back(dat);
                waveQueue.push(dat);
            }
            else
                que->pop();
        }
    }
    last_queue_size = cur_queue_size;
    cur_queue_size = waveQueue.size();

    //qDebug() << "len = " << que->size();
}


void MainWidget::flashwave()
{
    qDebug() << "cur_queue_size = " << cur_queue_size;
    qDebug() << "cur_len_per = " << cur_len_per;
    qDebug() << "WaveDataQue size = " << WaveDataQue->size() << endl;
    /*判断queue中数据长度满足一次刷新*/
    if(WaveDataQue->size() >= cur_len_per * peakNum)
    {
        if(isStop)
        {
            int num = peakNum * cur_len_per;
            while(num--) WaveDataQue->pop();
        }
        else
        {
            int i;
            QVector<QPointF> oldData = series->pointsVector();
            QVector<QPointF> flashdata;

            if (oldData.size() < WINDOW_MAX_X)
            {
                flashdata = series->pointsVector();
            }
            else
            {
                /* 添加之前老的数据到新的vector中，不复制最前的数据，即每次替换前面的数据
                 * 由于这里每次只添加1个数据，所以为1，使用时根据实际情况修改
                 */

                for (i = 1; i < oldData.size() - cur_len_per; ++i)
                {
                    flashdata.append(QPointF( i - 1 , oldData.at(i + cur_len_per).y()));
                }
            }

            qint64 size = flashdata.size();
            /* 这里表示插入新的数据，因为每次只插入1个，这里为i < 1,
             * 但为了后面方便插入多个数据，先这样写
             */
            if(lastShowpeakNum != ui->comboBox->currentIndex())//改变了显示的区域,重置显示
            {
                lastShowpeakNum = ui->comboBox->currentIndex();
                //flashdata.clear();
                series->clear();
                //chart->axisX()->setRange(0, WINDOW_MAX_X);//1s的数据显示范围
            }
            else
            {
                for(i = 0; i < cur_len_per; ++i)
                {
                    for(int j = 0; j < peakNum; ++j)
                    {
                        if(j == lastShowpeakNum) flashdata.append(QPointF(j + size, WaveDataQue->front()));
                        WaveDataQue->pop();
                    }
                }

            }
            series->replace(flashdata);
        }
    }



//        if(!isStop)//暂停显示
//        {
//            series->replace(flashdata);
//        }
//        else
//        {
//            clearWave = true;
//        }

//        if(clearWave & !isStop)//开始显示后，清除波形
//        {
//            clearWave = false;
//            series->clear();
//        }
//    }

}



void MainWidget::buttonSlot()
{
    if (QObject::sender() == ui->stopBtn) {
        if (!isStopping) {
            isStop = true;
            ui->stopBtn->setText("开始");

        } else {
            isStop = false;
            ui->stopBtn->setText("暂停");
            series->clear();//重新开始显示，清空曲线

        }
        isStopping = !isStopping;
    }
}

void MainWidget::tipSlot(QPointF position, bool isHovering)
{
    if (tip == 0)
        tip = new Callout(chart);

    if (isHovering) {
        tip->setText(QString("X: %1 \nY: %2 ").arg(position.x()).arg(position.y()));
        tip->setAnchor(position);
        tip->setZValue(11);
        tip->updateGeometry();
        tip->show();
    } else {
        tip->hide();
    }
}

