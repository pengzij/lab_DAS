#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QLabel>
#include <QChartView>
#include <QValueAxis>
#include <QVBoxLayout>
#include "PeakSearch.h"
#include <QWheelEvent>
using namespace QtCharts;


namespace Ui {
class Plot;
}

class Plot : public QWidget
{
    Q_OBJECT
public:
    explicit Plot(QWidget *parent = 0);
    ~Plot();
public:

    void loadData(int *peakPos, float *peakValue,unsigned int PeakNum, float *dataAll,unsigned int dataLen);
    void loadData(int x);

    void FreeMemory();

public slots:

    void slotPointHoverd(const QPointF &point, bool state);


signals:
    void ResearchPeakSignal();
    void ConfirmSearchPeakSignal();
public:

    QChart *m_chart;

    QLineSeries *m_series;
    QScatterSeries *m_scatLine;

    QValueAxis *axisX;
    QValueAxis *axisY;

    QLabel *m_valueLable;

protected:
    void wheelEvent(QWheelEvent *event);

private slots:
    void on_ChannelSelectBox_currentTextChanged(const QString &arg1);
    void on_ConfirmPeak_Btn_clicked();

    void on_ResearchPeak_Btn_clicked();

    bool readPeakData();

public:
    int* CH1Pos = NULL;
    int* CH2Pos = NULL;
    int* CH3Pos = NULL;

    unsigned int CH1PeakNum;
    unsigned int CH2PeakNum;
    unsigned int CH3PeakNum;

    float *CH1PeakValue = NULL;
    float *CH2PeakValue = NULL;
    float *CH3PeakValue = NULL;

    unsigned int lenOfChannel;
    float *CH1DataAll = NULL;
    float *CH2DataAll = NULL;
    float *CH3DataAll = NULL;

    float YMax;
    bool isSearchPeakAgain;

private:
    Ui::Plot *ui;
};

#endif // PLOT_H
