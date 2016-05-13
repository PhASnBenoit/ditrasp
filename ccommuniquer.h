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
#include <unistd.h>
#include <global.h>
#include "cmsg.h"

#define CARFIN '!'

typedef enum {
    AVANT_MISSION = 0,
    PENDANT_MISSION,
    APRES_MISSION
} T_ETAT_SYSTEM;

typedef enum {
    RECH_DEBUT_TRAME = 0,
    SAUVE_TRAME
} T_ETAT_COMM_DATA;

class CCommuniquer : public QObject
{
    Q_OBJECT

public:
    explicit CCommuniquer(QObject *parent = 0, CMsg *msg = 0);
    ~CCommuniquer();

private:
    QObject *mParent;
    CMsg *mMsg;
    QSerialPort *mPs;
    QFile *mFileCsv;
    QTimer *mTimer;
    QSharedMemory *mShm;
    T_Mes *mData;   // pointeur du segment de mémoire partagé
    int mTimerMesures;
    T_ETAT_SYSTEM mEtat;  // 0 avant mission    1 pendant mission     2 après mission
    T_ETAT_COMM_DATA mEtatData;  // gestion de la réception de car data
    QByteArray trame;  // permet de recevoir la trame de la liaison data

    int initPs(QSerialPort *serial);
    int protocole(QByteArray &qbaTrame, int lgTrame);
    US crc16(UC *tab, int nb);
    US retrouveCrc(QByteArray qbaCrc);

signals:
    void afficherTexte(QString aff);

private slots:
    void onMessReady(long type);    // message dans la file
    void onReadyRead();             // si réception par voie série
    void onTimer();                 // sauve les mesures

public slots:

};

#endif // CCOMMUNIQUER_H
