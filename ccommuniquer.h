#ifndef CCOMMUNIQUER_H
#define CCOMMUNIQUER_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSharedMemory>
#include <QDateTime>
#include <QTimer>
#include <QFile>
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
    QFile *mFileCsv;
    QTimer *timer;
    QSharedMemory *mShm;
    T_Mes *mData;   // pointeur du segment de mémoire partagé
    int initPs(QSerialPort *mPs);
    int protocole();

signals:

private slots:
    void onMessReady(long type);    // message dans la file
    void onReadyRead();             // si réception par voie série
    void onTimer();                 // sauve les mesures

public slots:

};

#endif // CCOMMUNIQUER_H
