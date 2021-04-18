/********************************************************************************
** Form generated from reading UI file 'waveform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAVEFORM_H
#define UI_WAVEFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>
#include <qchartview.h>

QT_BEGIN_NAMESPACE

class Ui_WaveForm
{
public:
    QtCharts::QChartView *lineGraphicsView_1;
    QtCharts::QChartView *lineGraphicsView_2;
    QtCharts::QChartView *lineGraphicsView_3;
    QLineEdit *lineEdit_1;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;

    void setupUi(QWidget *WaveForm)
    {
        if (WaveForm->objectName().isEmpty())
            WaveForm->setObjectName(QStringLiteral("WaveForm"));
        WaveForm->resize(1260, 660);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(WaveForm->sizePolicy().hasHeightForWidth());
        WaveForm->setSizePolicy(sizePolicy);
        lineGraphicsView_1 = new QtCharts::QChartView(WaveForm);
        lineGraphicsView_1->setObjectName(QStringLiteral("lineGraphicsView_1"));
        lineGraphicsView_1->setGeometry(QRect(0, 0, 1200, 220));
        sizePolicy.setHeightForWidth(lineGraphicsView_1->sizePolicy().hasHeightForWidth());
        lineGraphicsView_1->setSizePolicy(sizePolicy);
        lineGraphicsView_2 = new QtCharts::QChartView(WaveForm);
        lineGraphicsView_2->setObjectName(QStringLiteral("lineGraphicsView_2"));
        lineGraphicsView_2->setGeometry(QRect(0, 220, 1200, 220));
        sizePolicy.setHeightForWidth(lineGraphicsView_2->sizePolicy().hasHeightForWidth());
        lineGraphicsView_2->setSizePolicy(sizePolicy);
        lineGraphicsView_3 = new QtCharts::QChartView(WaveForm);
        lineGraphicsView_3->setObjectName(QStringLiteral("lineGraphicsView_3"));
        lineGraphicsView_3->setGeometry(QRect(0, 440, 1200, 220));
        sizePolicy.setHeightForWidth(lineGraphicsView_3->sizePolicy().hasHeightForWidth());
        lineGraphicsView_3->setSizePolicy(sizePolicy);
        lineEdit_1 = new QLineEdit(WaveForm);
        lineEdit_1->setObjectName(QStringLiteral("lineEdit_1"));
        lineEdit_1->setGeometry(QRect(1200, 110, 50, 20));
        lineEdit_2 = new QLineEdit(WaveForm);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(1200, 330, 50, 20));
        lineEdit_3 = new QLineEdit(WaveForm);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(1200, 550, 50, 20));

        retranslateUi(WaveForm);

        QMetaObject::connectSlotsByName(WaveForm);
    } // setupUi

    void retranslateUi(QWidget *WaveForm)
    {
        WaveForm->setWindowTitle(QApplication::translate("WaveForm", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class WaveForm: public Ui_WaveForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAVEFORM_H
