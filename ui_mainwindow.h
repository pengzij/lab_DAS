/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTextEdit *StateText;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QPushButton *SearchPeak_Button;
    QGridLayout *gridLayout_2;
    QPushButton *WatchPeak_Button;
    QGridLayout *gridLayout_3;
    QPushButton *Demodu_Button;
    QGridLayout *gridLayout_4;
    QPushButton *DemoduStop_Button;
    QGridLayout *gridLayout_5;
    QPushButton *Play_Button;
    QLabel *regionText;
    QGridLayout *gridLayout_6;
    QTextEdit *regionEidt;
    QGridLayout *gridLayout_7;
    QCheckBox *save_checkBox;
    QPushButton *Sound_Button;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(733, 525);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        StateText = new QTextEdit(centralWidget);
        StateText->setObjectName(QStringLiteral("StateText"));
        StateText->setGeometry(QRect(9, 67, 711, 411));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font.setPointSize(10);
        font.setBold(false);
        font.setWeight(50);
        StateText->setFont(font);
        StateText->setReadOnly(true);
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(70, 20, 655, 31));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        SearchPeak_Button = new QPushButton(verticalLayoutWidget);
        SearchPeak_Button->setObjectName(QStringLiteral("SearchPeak_Button"));

        gridLayout->addWidget(SearchPeak_Button, 0, 0, 1, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        WatchPeak_Button = new QPushButton(verticalLayoutWidget);
        WatchPeak_Button->setObjectName(QStringLiteral("WatchPeak_Button"));

        gridLayout_2->addWidget(WatchPeak_Button, 0, 0, 1, 1);


        gridLayout->addLayout(gridLayout_2, 0, 1, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        Demodu_Button = new QPushButton(verticalLayoutWidget);
        Demodu_Button->setObjectName(QStringLiteral("Demodu_Button"));

        gridLayout_3->addWidget(Demodu_Button, 0, 0, 1, 1);


        gridLayout->addLayout(gridLayout_3, 0, 2, 1, 1);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setSpacing(6);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        DemoduStop_Button = new QPushButton(verticalLayoutWidget);
        DemoduStop_Button->setObjectName(QStringLiteral("DemoduStop_Button"));

        gridLayout_4->addWidget(DemoduStop_Button, 0, 0, 1, 1);


        gridLayout->addLayout(gridLayout_4, 0, 3, 1, 1);

        gridLayout_5 = new QGridLayout();
        gridLayout_5->setSpacing(6);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        Play_Button = new QPushButton(verticalLayoutWidget);
        Play_Button->setObjectName(QStringLiteral("Play_Button"));

        gridLayout_5->addWidget(Play_Button, 0, 0, 1, 1);


        gridLayout->addLayout(gridLayout_5, 0, 5, 1, 1);

        regionText = new QLabel(verticalLayoutWidget);
        regionText->setObjectName(QStringLiteral("regionText"));

        gridLayout->addWidget(regionText, 0, 6, 1, 1);

        gridLayout_6 = new QGridLayout();
        gridLayout_6->setSpacing(6);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        regionEidt = new QTextEdit(verticalLayoutWidget);
        regionEidt->setObjectName(QStringLiteral("regionEidt"));
        regionEidt->setEnabled(true);
        regionEidt->setMinimumSize(QSize(40, 25));
        regionEidt->setMaximumSize(QSize(40, 25));

        gridLayout_6->addWidget(regionEidt, 0, 0, 1, 1);

        gridLayout_7 = new QGridLayout();
        gridLayout_7->setSpacing(6);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        save_checkBox = new QCheckBox(verticalLayoutWidget);
        save_checkBox->setObjectName(QStringLiteral("save_checkBox"));

        gridLayout_7->addWidget(save_checkBox, 0, 0, 1, 1);


        gridLayout_6->addLayout(gridLayout_7, 0, 1, 1, 1);


        gridLayout->addLayout(gridLayout_6, 0, 7, 1, 1);

        Sound_Button = new QPushButton(verticalLayoutWidget);
        Sound_Button->setObjectName(QStringLiteral("Sound_Button"));

        gridLayout->addWidget(Sound_Button, 0, 4, 1, 1);


        verticalLayout->addLayout(gridLayout);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        SearchPeak_Button->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213\345\257\273\345\263\260", Q_NULLPTR));
        WatchPeak_Button->setText(QApplication::translate("MainWindow", "\346\237\245\347\234\213\345\263\260\345\200\274", Q_NULLPTR));
        Demodu_Button->setText(QApplication::translate("MainWindow", "\350\247\243\350\260\203", Q_NULLPTR));
        DemoduStop_Button->setText(QApplication::translate("MainWindow", "\345\201\234\346\255\242\350\247\243\350\260\203", Q_NULLPTR));
        Play_Button->setText(QApplication::translate("MainWindow", "\346\222\255\346\224\276", Q_NULLPTR));
        regionText->setText(QApplication::translate("MainWindow", " \351\200\211\345\214\272", Q_NULLPTR));
        save_checkBox->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256\345\255\230\345\202\250", Q_NULLPTR));
        Sound_Button->setText(QApplication::translate("MainWindow", "\345\243\260\351\237\263\345\275\225\345\210\266", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
