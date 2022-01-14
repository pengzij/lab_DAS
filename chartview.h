#ifndef __CHARTVIEW_H__
#define __CHARTVIEW_H__

#include <QChartView>
#include <QRubberBand>
#include <iostream>
#include <QDebug>
#include <memory>

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
public:
    ChartView(QChart *chart, std::shared_ptr<bool> istop, QWidget *parent = 0);
    bool isClicking;
    bool m_ctrlPress;
    bool resetZoom;
protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:

    std::shared_ptr<bool> isStop;
    int xOld;
    int yOld;

signals:
    void PressMouse();
    void ReleaseMouse();
};

#endif /* __CHARTVIEW_H__ */
