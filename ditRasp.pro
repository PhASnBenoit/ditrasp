#-------------------------------------------------
#
# Project created by QtCreator 2016-02-27T15:26:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ditRasp
TEMPLATE = app

target.path = /home/pi
INSTALLS += target

SOURCES += main.cpp\
        mainwindow.cpp \
    pc_communiquer.cpp \
    pc_controlercamera.cpp \
    pc_incruster.cpp \
    cmsg.cpp \
    cspi.cpp \
    ccapteuri2clm76_temp.cpp \
    cdevicespimax7456.cpp \
    ci2c.cpp


HEADERS  += mainwindow.h \
    pc_communiquer.h \
    pc_controlercamera.h \
    pc_incruster.h \
    cmsg.h \
    global.h \
    cspi.h \
    ccapteuri2clm76_temp.h \
    cdevicespimax7456.h \
    ci2c.h

FORMS    += mainwindow.ui
