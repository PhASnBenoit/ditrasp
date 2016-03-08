#ifndef CCONTROLERCAMERA_H
#define CCONTROLERCAMERA_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QTcpSocket>
#include <QHostAddress>
#include <stdio.h>
#include "cmsg.h"

/*
#define ON "01"
#define OFF "00"
#define START "01"
#define STOP "00"
// mode
#define CAMERA "00"
#define PHOTO "01"
#define BURST "02"
#define TIMELAPS1 "03"
#define TIMELAPS2 "04"
// orientation ???
#define HEAD_UP "00"
#define HEAD_DOWN "01"
// résolution vidéo
#define WVGA_60 "00"
#define WVGA_120 "01"
#define R720_30 "02"
#define R720_60 "03"
// résolution photo
#define R12MPWIDE "00"
#define R8MPMEDIUM "01"
#define R5MPWIDE "02"
#define R5MPMEDIUM "03"
// Timer
#define T05SEC "00"
#define T1SEC "01"
#define T2SEC "02"
#define T5SEC "03"
#define T10SEC "04"
#define T30SEC "05"
#define T60SEC "06"
// volume
#define ARRET "00"
#define V70POURCENT "01"
#define V100POURCENT "02"
*/
/////////////////////////////////////////////////////////////////////
class CControlerCamera : public QObject
{
    Q_OBJECT

public:
    explicit CControlerCamera(QObject *parent = 0, CMsg *msg = 0);
    ~CControlerCamera();

private:
    CMsg *pMsg; //
    QHostAddress mAdrIP;     // adr IP de la caméra
    int mPort;
    QString mMdp;            // Mot de passe WIFI
    QTcpSocket *mSock;

signals:

private slots:
    void onMessReady(long type);
    void onReadyRead();
    void onConnected();
    void onDisconnected();

public slots:

};

#endif // CCONTROLERCAMERA_H
