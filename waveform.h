#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QLabel>
#include <QChartView>
#include <QValueAxis>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QBarSet>
#include <QBarSeries>
#include <QVector>
#include "Config.h"
#include "waveplot.h"
#define WAVE_WIDGET_LENGTH (4000)


using namespace QtCharts;

namespace Ui {
class WaveForm;
}

class WaveForm : public QWidget
{
    Q_OBJECT

public:
    explicit WaveForm(QWidget *parent = 0);
    ~WaveForm();
public:
    void WidgetInit();
    void paramInit(Config *cfig);
    void loadExampleData(int index);


protected:

    void mouseDoubleClickEvent(QMouseEvent *event);

public:
    QChart *lineChart_1;
    QChart *lineChart_2;
    QChart *lineChart_3;

    QLineSeries *lineSeries_1;
    QLineSeries *lineSeries_2;
    QLineSeries *lineSeries_3;

    QValueAxis *axisX_Line_1;
    QValueAxis *axisX_Line_2;
    QValueAxis *axisX_Line_3;

    QValueAxis *axisY_Line_1;
    QValueAxis *axisY_Line_2;
    QValueAxis *axisY_Line_3;

    WavePlot *plotWidget = NULL;

public:
    unsigned int peakNum;
    float ValueMax;
    float ValueMin;
    bool isCalculatePhase;
    uint16_t waveLength;


private slots:

    void on_lineEdit_1_returnPressed();

    void on_lineEdit_2_returnPressed();

    void on_lineEdit_3_returnPressed();

signals:
    void SelectFBGChanged(int FBG1,int FBG2,int FBG3);


private:
    Ui::WaveForm *ui;
};

#endif // WAVEFORM_H
