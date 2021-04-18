#ifndef WAVEPLOT_H
#define WAVEPLOT_H

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
#include <QPointF>
#include <QVector>

using namespace QtCharts;
namespace Ui {
class WavePlot;
}

class WavePlot : public QWidget
{
    Q_OBJECT

public:
    explicit WavePlot(QWidget *parent = 0);
    ~WavePlot();
    void loadWaveData(QVector<QPointF> &points);

public:
    void WidgetInit();
    void setParam(int wavelen,float min, float max);


public slots:
    void showMenu(const QPoint &pos);
    void restoreAxis();

public:
    int WaveLength;
    float ValueMin;
    float ValueMax;

    QChart *m_chart;
    QLineSeries *m_series;
    QScatterSeries *m_scatSeries;
    QValueAxis *axisX;
    QValueAxis *axisY;

private:
    Ui::WavePlot *ui;
};

#endif // WAVEPLOT_H
