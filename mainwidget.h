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

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void initUI(int );
    void getpeakNum(int);

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
    void windowave();
};

#endif // MAINWIDGET_H
