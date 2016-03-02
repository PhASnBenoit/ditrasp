#ifndef PC_COMMUNIQUER_H
#define PC_COMMUNIQUER_H

#include <QObject>
//#include <QSerialPortInfo>
//#include <QSerialPort>
#include <stdio.h>
#include "cmsg.h"

class PC_Communiquer : public QObject
{
    Q_OBJECT

public:
    explicit PC_Communiquer(QObject *parent = 0, CMsg *msg = 0);

private:
    CMsg *pMsg;
//    QSerialPortInfo *psi;
//    QSerialPort *ps;

signals:

private slots:
    void onMessReady(long type);

public slots:

};

#endif // PC_COMMUNIQUER_H
