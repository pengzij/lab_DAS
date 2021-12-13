/********************************************************************************
** Form generated from reading UI file 'plot.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOT_H
#define UI_PLOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <qchartview.h>

QT_BEGIN_NAMESPACE

class Ui_Plot
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QComboBox *ChannelSelectBox;
    QPushButton *ResearchPeak_Btn;
    QPushButton *ConfirmPeak_Btn;
    QtCharts::QChartView *graphicsView;

    void setupUi(QWidget *Plot)
    {
        if (Plot->objectName().isEmpty())
            Plot->setObjectName(QStringLiteral("Plot"));
        Plot->resize(862, 365);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Plot->sizePolicy().hasHeightForWidth());
        Plot->setSizePolicy(sizePolicy);
        Plot->setMinimumSize(QSize(75, 20));
        Plot->setWindowOpacity(1);
        vboxLayout = new QVBoxLayout(Plot);
        vboxLayout->setSpacing(4);
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        vboxLayout->setContentsMargins(0, 6, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout->setContentsMargins(9, -1, 9, -1);
        horizontalSpacer = new QSpacerItem(36, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        ChannelSelectBox = new QComboBox(Plot);
        ChannelSelectBox->setObjectName(QStringLiteral("ChannelSelectBox"));
        sizePolicy.setHeightForWidth(ChannelSelectBox->sizePolicy().hasHeightForWidth());
        ChannelSelectBox->setSizePolicy(sizePolicy);
        ChannelSelectBox->setMinimumSize(QSize(75, 20));

        horizontalLayout->addWidget(ChannelSelectBox);

        ResearchPeak_Btn = new QPushButton(Plot);
        ResearchPeak_Btn->setObjectName(QStringLiteral("ResearchPeak_Btn"));

        horizontalLayout->addWidget(ResearchPeak_Btn);

        ConfirmPeak_Btn = new QPushButton(Plot);
        ConfirmPeak_Btn->setObjectName(QStringLiteral("ConfirmPeak_Btn"));

        horizontalLayout->addWidget(ConfirmPeak_Btn);


        vboxLayout->addLayout(horizontalLayout);

        graphicsView = new QtCharts::QChartView(Plot);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setAutoFillBackground(true);
        graphicsView->setLineWidth(0);

        vboxLayout->addWidget(graphicsView);


        retranslateUi(Plot);

        QMetaObject::connectSlotsByName(Plot);
    } // setupUi

    void retranslateUi(QWidget *Plot)
    {
        Plot->setWindowTitle(QApplication::translate("Plot", "Form", Q_NULLPTR));
        ChannelSelectBox->clear();
        ChannelSelectBox->insertItems(0, QStringList()
         << QApplication::translate("Plot", "1", Q_NULLPTR)
         << QApplication::translate("Plot", "2", Q_NULLPTR)
         << QApplication::translate("Plot", "3", Q_NULLPTR)
        );
        ResearchPeak_Btn->setText(QApplication::translate("Plot", "\351\207\215\346\226\260\345\257\273\345\263\260", Q_NULLPTR));
        ConfirmPeak_Btn->setText(QApplication::translate("Plot", "\347\241\256\350\256\244\345\263\260\345\200\274", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Plot: public Ui_Plot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOT_H
