#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtMath>
#include <QList>
#include <sys/time.h>


#define TIMEINTERVAL 25//定时器时间间隔 刷新频率50Hz
#define FRE 30//频率，因为是毫秒单位，30KHz
#define LEN_PER FRE * TIMEINTERVAL//单次刷新显示数目
#define WINDOW_MAX_X 10000 //窗口x轴范围
#define SHOWDATA_MAX WINDOW_MAX_X * 10//历史数据最大长度
#define XSACLE 10//x轴时间刻度
#define SENDSIZE 9000

static int ii = 0;

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    chart(new QChart),
    tip(0),
    timer(new QTimer),
    timer2(new QTimer),
    count(0),
    isStopping(false),
    isStop(false),
    isStopSetRange(false),
    wavecase(0),
    cur_x(0),
    last_queue_size(0),
    cur_queue_size(0),
    cur_len_per(LEN_PER),
    isUpdataing(false),
    clearWave(true),
    lastShowpeakNum(-1)//初始化为不可能的数据
{
    ui->setupUi(this);
    timer->setInterval(1000);//1s定时
    timer2->setInterval(TIMEINTERVAL);
}

MainWidget::~MainWidget()
{
    timer->stop();
    timer2->stop();

    delete series;
    delete tip;
    delete chart;
    delete timer;
    delete timer2;
    delete chartView;
    delete ui;
    qDebug() << "61" << endl;
}

void MainWidget::getpeakNum(int peaknum)
{
    peakNum = peaknum;
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

void MainWidget::initUI(int peaknum)
{
    peakNum = peaknum;
    qDebug() << "initUI peaknum = " << peaknum <<endl;
    QString intostring;
    for (int i = 1; i <= peaknum; i++) {
        ui->comboBox->addItem(intostring.setNum(i));
    }

    initChart();
    //timer->setInterval(10);//10ms 刷新一个点
    //timer->start();
    initSlot();
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
    //connect(timer, SIGNAL(timeout()), this, SLOT(timerSlot()));
    //connect(chartView, SIGNAL(PressMouse()), this, SLOT(stopSetRange()) );
    //connect(chartView, SIGNAL(ReleaseMouse()), this, SLOT(startime()));
    connect(timer, SIGNAL(timeout()), this, SLOT(startSetRange()));
    connect(timer2, SIGNAL(timeout()), this, SLOT(flashwave()));
    connect(ui->stopBtn, SIGNAL(clicked(bool)), this, SLOT(buttonSlot()));
    connect(series, SIGNAL(hovered(QPointF, bool)), this, SLOT(tipSlot(QPointF,bool)));
}

void MainWidget::stopSetRange()
{
    isStopSetRange = true;
}

void MainWidget::startime()
{
    timer->setInterval(1000);
    timer->start();
}

void MainWidget::startSetRange()
{
    isStopSetRange = false;
    timer->stop();
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



void MainWidget::windowave()
{
    if(showDataList.size() > 0)//每1ms 判断是否需要更新曲线
    {

        QList<QPointF> a;
        for(int i =0 ; i < 10; i++)
       {
            a.append(QPointF(i + ii, 1));
        }
        ii += 10;
        series->replace(a);
        *series << a;
        showDataList.clear();

    }

    switch(wavecase)
    {
    case 0://窗口重置
        showDataList.clear();
        showData.clear();
        series->clear();
        chart->axisX()->setRange(0, WINDOW_MAX_X);//1s的数据显示范围
        cur_show_x = 0;
        wavecase = 1;
        break;

    case 1://开始绘制曲线
        cur_show_x = series->count();//获取当前曲线长度
        if(cur_show_x >= WINDOW_MAX_X)
        {
            cur_show_x = WINDOW_MAX_X;//如果曲线长度已经超过显示窗口范围
            wavecase = 2;
            break;
        }
        wavecase = 1;
        break;

    case 2:
        cur_show_x += LEN_PER;
        if(series->count() >= cur_show_x)
            chart->axisX()->setRange(cur_show_x - WINDOW_MAX_X, cur_show_x);//每毫秒显示区域右移LEN_PER_MSEC
        else
        {
            cur_show_x -= LEN_PER;//如果数据到头了，还原等待新的数据到达
        }

        if(series->count() >= SHOWDATA_MAX * 3 / 2)//历史数据最大值的3 / 2
        {
            series->removePoints(0, SHOWDATA_MAX / 2);

        }



    default:
        wavecase = 0;
        break;
    }
    if(lastShowpeakNum != ui->comboBox->currentIndex())//改变了显示的区域,重置显示
    {
        lastShowpeakNum = ui->comboBox->currentIndex();//更新
        wavecase = 0;
    }
}

void MainWidget::flashwave()
{
    qDebug() << "cur_queue_size = " << cur_queue_size;
    /*根据发送数据的速度更新显示速度, 为避免显示速度过快，设置增加步进小于减少步进*/
//    if(cur_queue_size > SENDSIZE )
//        cur_len_per += LEN_PER / 30;
//    else
//    {
//        cur_len_per -= LEN_PER / 30;
//        if(cur_len_per <= 0)
//            cur_len_per = 0;
//    }
    cur_len_per = SENDSIZE / 9;
    qDebug() << "cur_len_per = " << cur_len_per;
    /*判断queue中数据长度满足一次刷新*/
    if(waveQueue.size() >= cur_len_per)
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
                flashdata.append(QPointF(i + size, waveQueue.front()));
                waveQueue.pop();
            }

        }
        if(!isStop)//暂停显示
        {
            series->replace(flashdata);
        }
        else
        {
            clearWave = true;
        }

        if(clearWave & !isStop)//开始显示后，清除波形
        {
            clearWave = false;
            series->clear();
        }
//        if(flashdata.size() > WINDOW_MAX_X && chartView->isClicking == false && !isStopSetRange)
//        {
//            chart->axisX()->setRange(size - WINDOW_MAX_X, size);
//        }
//        else if(flashdata.size() > WINDOW_MAX_X && chartView->isClicking)
//        {
//            isStopSetRange = true;
//            timer->setInterval(1000);//停止点击鼠标后1s，定位到最新数据显示
//            timer->start();
//        }
    }

}


void MainWidget::timerSlot()
{
    if (QObject::sender() == timer) {
        updateWave();
    }
}

void MainWidget::updateWave()
{
    int i;
    QVector<QPointF> oldData = series->pointsVector();
    QVector<QPointF> data;

    if (oldData.size() < 10000) {
        data = series->pointsVector();
    } else {
        /* 添加之前老的数据到新的vector中，不复制最前的数据，即每次替换前面的数据
         * 由于这里每次只添加1个数据，所以为1，使用时根据实际情况修改
         */
        for (i = 1; i < oldData.size() - 200; ++i) {

            data.append(QPointF(i - 1 , oldData.at(i + 200).y()));

        }
    }

    qint64 size = data.size();

    /* 这里表示插入新的数据，因为每次只插入1个，这里为i < 1,
     * 但为了后面方便插入多个数据，先这样写
     */
    for(i = 0; i < 200; ++i){
        data.append(QPointF(i + size, showData[i]));
        count++;
    }

    series->replace(data);


}

void MainWidget::buttonSlot()
{
    if (QObject::sender() == ui->stopBtn) {
        if (!isStopping) {
            //timer2->stop();
            isStop = true;
            ui->stopBtn->setText("开始");

        } else {
            isStop = false;
            ui->stopBtn->setText("暂停");
            //timer2->start();
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

