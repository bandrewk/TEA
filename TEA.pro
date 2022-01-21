#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T12:00:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TEA
TEMPLATE = app


SOURCES += main.cpp\
    frmabout.cpp \
        frmmain.cpp \
    device.cpp \
    frmvideocapture.cpp

HEADERS  += frmmain.h \
    device.h \
    frmabout.h \
    frmvideocapture.h

FORMS    += frmmain.ui \
    frmabout.ui \
    frmvideocapture.ui
