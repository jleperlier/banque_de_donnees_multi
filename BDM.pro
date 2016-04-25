#-------------------------------------------------
#
# Project created by QtCreator 2016-03-04T11:21:45
#
#-------------------------------------------------

QT       += core gui
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BDM
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    complex.cpp \
    fft.cpp \
    song.cpp

HEADERS  += mainwindow.h \
    complex.h \
    fft.h \
    song.h

FORMS    += mainwindow.ui
