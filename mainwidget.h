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

#include "Config.h"
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
#define WINDOW_MAX_Y 3.0

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    //fre:30对应30kHz tmInterval:20ms时间间隔对应刷新频率50Hz
    MainWidget(const shared_ptr<GetConfig> gcfg = nullptr, shared_ptr<CirQueue<float>> wavedataque = nullptr, int tmInterval = 20, int window_max_x = 10000, int xScale = 10, QWidget *parent = 0);
    ~MainWidget();
    void initUI();
    int& setSENDSIZE(int &);


private:
    int TIMEINTERVAL;//定时器时间间隔
    int FRE;//频率
    int LEN_PER;//单次刷新显示数目
    int WINDOW_MAX_X;//窗口x轴范围
    int SHOWDATA_MAX;//历史数据最大长度
    int XSACLE;//x轴时间刻度
    shared_ptr<CirQueue<float>> WaveDataQue;
    QVector<float> showData;
    list<QPointF> showDataListSTL;
    QList<QPointF> showDataList;

    Ui::MainWidget *ui;

    ChartView *chartView;
    QChart *chart;
    Callout *tip;
    QLineSeries *series;

    queue<float> waveQueue;

    QTimer *timer2;

    quint16 count;

    bool isUpdataing;
    bool isStopping;
    bool clearWave;
    std::shared_ptr<bool> isStop;

    int last_queue_size;
    int cur_queue_size;
    int cur_len_per;
    int peakNum;
    int lastShowpeakNum;

    int wavecase;
    int cur_x;
    int cur_show_x;
    int SENDSIZE;
    pair<double, double> cur_windowRange_x;
    pair<double, double> cur_windowRange_y;

    void wheelEvent(QWheelEvent *event);

    void initChart();
    void initSlot();
    void sendData(CirQueue<float>*);
    void zoom_X(pair<double, double>& pair_x, const double zoomValue);
    void zoom_Y(pair<double, double>& pair_y, const double zoomValue);

public slots:
    void updateData(CirQueue<float>* , int);

private slots:
    void buttonSlot();
    void tipSlot(QPointF position, bool isHovering);
    void flashwave();
};

#endif // MAINWIDGET_H
