#ifndef CCOMMUNIQUER_H
#define CCOMMUNIQUER_H

#include <QObject>
#include <QDebug>
//#include <QSerialPortInfo>
//#include <QSerialPort>
#include <stdio.h>
#include "cmsg.h"

class CCommuniquer : public QObject
{
    Q_OBJECT

public:
    explicit CCommuniquer(QObject *parent = 0, CMsg *msg = 0);

private:
    CMsg *pMsg;
//    QSerialPortInfo *psi;
//    QSerialPort *ps;

signals:

private slots:
    void onMessReady(long type);

public slots:

};

#endif // CCOMMUNIQUER_H
