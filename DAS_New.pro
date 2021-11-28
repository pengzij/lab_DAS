#-------------------------------------------------
#
# Project created by QtCreator 2019-08-02T14:35:47
#
#-------------------------------------------------

QT       += core gui
QT       += gui
QT       += concurrent
QT       += svg
QT       += opengl
QT       += network
QT       += serialport
QT       += charts
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = DAS_New
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    wzserialport.cpp \
    Config.cpp \
    PeakSearch.cpp \
    USBCtrl.cpp \
    demodulation.cpp \
    udpconnect.cpp \
    plot.cpp \
    waveform.cpp \
    waveplot.cpp \
    callout.cpp \
    chartview.cpp \
    mainwidget.cpp

HEADERS  += mainwindow.h \
    wzserialport.h \
    Config.h \
    PeakSearch.h \
    USBCtrl.h \
    sdk/CyUSB3000.h \
    sdk/CU3AD.h \
    demodulation.h \
    udpconnect.h \
    plot.h \
    waveform.h \
    CirQueue.h \
    waveplot.h \
    CirQueue1.h \
    wavhead.h \
    callout.h \
    chartview.h \
    mainwidget.h

FORMS    += mainwindow.ui \
    plot.ui \
    waveform.ui \
    waveplot.ui \
    mainwidget.ui

#openmp
QMAKE_CXXFLAGS+= -openmp


win32: LIBS += -L$$PWD/sdk/ -lCU3AD
#win32: LIBS += user32.lib
INCLUDEPATH += $$PWD/sdk
DEPENDPATH += $$PWD/sdk

unix|win32: LIBS += -lgdi32

unix|win32: LIBS += -lwinmm

UI_DIR=./UI
