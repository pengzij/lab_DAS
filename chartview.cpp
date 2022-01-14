#include "chartview.h"

ChartView::ChartView(QChart *chart, std::shared_ptr<bool> istop, QWidget *parent) :
    QChartView(chart, parent),
    isClicking(false),
    xOld(0), yOld(0),
    isStop(istop),
    m_ctrlPress(false),
    resetZoom(false)
{
    setRubberBand(QChartView::RectangleRubberBand);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if(*isStop)
    {
        if (event->button() & Qt::LeftButton) {
            //emit PressMouse();
            isClicking = true;
        } else if (event->button() & Qt::RightButton) {
            chart()->zoomReset();//重置窗口缩放
            resetZoom = true;
        }

        QChartView::mousePressEvent(event);
    }

}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if(*isStop)
    {
        int x, y;

        if (isClicking) {
            if (xOld == 0 && yOld == 0) {

            } else {
                x = event->x() - xOld;
                y = event->y() - yOld;
                chart()->scroll(-x, y);
            }

            xOld = event->x();
            yOld = event->y();

            return;
        }

        QChartView::mouseMoveEvent(event);
    }

}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(*isStop)
    {
        if (isClicking) {
            xOld = yOld = 0;
            isClicking = false;
           // emit ReleaseMouse();
        }

        /* Disable original right click event */
        if (!(event->button() & Qt::RightButton)) {
            QChartView::mouseReleaseEvent(event);
        }
    }

}



void ChartView::keyPressEvent(QKeyEvent *event)
{
    if(*isStop)
    {
        switch (event->key()) {
        case Qt::Key_Left:
            chart()->scroll(-10, 0);
            break;
        case Qt::Key_Right:
            chart()->scroll(10, 0);
            break;
        case Qt::Key_Up:
            chart()->scroll(0, 10);
            break;
        case Qt::Key_Down:
            chart()->scroll(0, -10);
            break;
        case Qt::Key_Control:
            //qDebug() << "crtl pressed" << endl;
            m_ctrlPress = true;
            break;
        default:
            keyPressEvent(event);
            break;
        }
        //qDebug() << "m_ctrlpress = " << m_ctrlPress << endl;
    }

}

void ChartView::keyReleaseEvent(QKeyEvent *event)
{
    if(*isStop)
    {
        if(event->key() == Qt::Key_Control)
            m_ctrlPress = false;
    }
}
