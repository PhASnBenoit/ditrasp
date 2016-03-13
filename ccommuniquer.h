#ifndef CCOMMUNIQUER_H
#define CCOMMUNIQUER_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <stdio.h>
#include <global.h>
#include "cmsg.h"

class CCommuniquer : public QObject
{
    Q_OBJECT

public:
    explicit CCommuniquer(QObject *parent = 0, CMsg *msg = 0);
    ~CCommuniquer();

private:
    QObject *pParent;
    CMsg *pMsg;
    QSerialPort *mPs;
    int initPs();
    int protocole();

signals:

private slots:
    void onMessReady(long type);    // message dans la file
    void onReadyRead();             // si réception par voie série

public slots:

};

#endif // CCOMMUNIQUER_H
