#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QChart>
#include <QLineSeries>
#include <QList>
#include <QVector>
#include <vector>
#include <queue>
#include <QTimer>
#include <list>

#include "chartview.h"
#include "callout.h"
#include "CirQueue.h"

using namespace std;
QT_CHARTS_USE_NAMESPACE

//#define TIMEINTERVAL 25//定时器时间间隔 刷新频率50Hz
//#define FRE 30//频率，因为是毫秒单位，30KHz
//#define LEN_PER FRE * TIMEINTERVAL//单次刷新显示数目
//#define WINDOW_MAX_X 10000 //窗口x轴范围
//#define SHOWDATA_MAX WINDOW_MAX_X * 10//历史数据最大长度
//#define XSACLE 10//x轴时间刻度

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    //fre:30对应30kHz tmInterval:25ms时间间隔对应刷新频率50Hz
    MainWidget(int fre = 30, int tmInterval = 25, int window_max_x = 10000, int xScale = 10, QWidget *parent = 0);
    ~MainWidget();
    void initUI(int );
    void getpeakNum(int);


private:
    int TIMEINTERVAL;//定时器时间间隔
    int FRE;//频率
    int LEN_PER;//单次刷新显示数目
    int WINDOW_MAX_X;//窗口x轴范围
    int SHOWDATA_MAX;//历史数据最大长度
    int XSACLE;//x轴时间刻度


private:
    void wheelEvent(QWheelEvent *event);

    void initChart();
    void initSlot();
    void sendData(CirQueue<float>* );

    QVector<float> showData;
    list<QPointF> showDataListSTL;
    QList<QPointF> showDataList;

    Ui::MainWidget *ui;

    ChartView *chartView;
    QChart *chart;
    Callout *tip;
    QLineSeries *series;

    queue<float> waveQueue;

    QTimer *timer;
    QTimer *timer2;

    quint16 count;

    bool isUpdataing;
    bool isStopping;
    bool clearWave;
    bool isStop;
    bool isStopSetRange;

    int last_queue_size;
    int cur_queue_size;
    int cur_len_per;
    int peakNum;
    int lastShowpeakNum;

    int wavecase;
    int cur_x;
    int cur_show_x;

public slots:

    void updateData(CirQueue<float>* , int);

private slots:

    void updateWave();
    void timerSlot();
    void stopSetRange();
    void startime();
    void startSetRange();
    void buttonSlot();
    void tipSlot(QPointF position, bool isHovering);
    void flashwave();
};

#endif // MAINWIDGET_H
