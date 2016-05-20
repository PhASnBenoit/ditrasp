#-------------------------------------------------
#
# Project created by QtCreator 2016-02-27T15:26:58
#
#-------------------------------------------------

QT       += core gui serialport network qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ditRasp
TEMPLATE = app

target.path = /home/pi
INSTALLS += target

SOURCES += main.cpp\
        mainwindow.cpp \
    cmsg.cpp \
    cspi.cpp \
    ccapteuri2clm76_temp.cpp \
    cdevicespimax7456.cpp \
    ci2c.cpp \
    ccapteuri2chmc5883_comp.cpp \
    ccontrolercamera.cpp \
    ccommuniquer.cpp \
    cincruster.cpp \
    ccapteurserialgps.cpp \
    cserialport.cpp \
    ccapteuri2chtu21d_humtemp.cpp \
    cgpio.cpp \
    ccapteurgpioping_dist.cpp


HEADERS  += mainwindow.h \
    cmsg.h \
    global.h \
    cspi.h \
    ccapteuri2clm76_temp.h \
    cdevicespimax7456.h \
    ci2c.h \
    ccapteuri2chmc5883_comp.h \
    ccontrolercamera.h \
    ccommuniquer.h \
    cincruster.h \
    ccapteurserialgps.h \
    cserialport.h \
    ccapteuri2chtu21d_humtemp.h \
    cgpio.h \
    ccapteurgpioping_dist.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    config.ini
