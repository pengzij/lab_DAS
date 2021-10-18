/********************************************************************************
** Form generated from reading UI file 'waveplot.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAVEPLOT_H
#define UI_WAVEPLOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>
#include <qchartview.h>

QT_BEGIN_NAMESPACE

class Ui_WavePlot
{
public:
    QtCharts::QChartView *graphicsView;

    void setupUi(QWidget *WavePlot)
    {
        if (WavePlot->objectName().isEmpty())
            WavePlot->setObjectName(QStringLiteral("WavePlot"));
        WavePlot->resize(1200, 300);
        graphicsView = new QtCharts::QChartView(WavePlot);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setGeometry(QRect(0, 0, 1200, 300));

        retranslateUi(WavePlot);

        QMetaObject::connectSlotsByName(WavePlot);
    } // setupUi

    void retranslateUi(QWidget *WavePlot)
    {
        WavePlot->setWindowTitle(QApplication::translate("WavePlot", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class WavePlot: public Ui_WavePlot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAVEPLOT_H
