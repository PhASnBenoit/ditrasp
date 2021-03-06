#ifndef CINCRUSTER_H
#define CINCRUSTER_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <stdio.h>
#include "cmsg.h"
#include "global.h"
#include "cdevicespimax7456.h"

typedef struct {
    char texte[50]; // ce champs doit rester en premier !
    int r,c;  // column, row
} T_Aff;

typedef struct {
    T_Aff c[9];  // val et pos d'incrustation
} T_AffInc;

class CIncruster : public QObject
{
    Q_OBJECT

public:
    explicit CIncruster(QObject *parent = 0, CMsg *msg = 0, int interval = 1000);
    ~CIncruster();

private:
    CMsg *mMsg;
    QTimer *mTimer;
    QSharedMemory *mShm;   // pointeur de gestion de l'objet mémoire partagé
    const T_Mes *mData;   // acces à la mémoire partagée
    int mNbCapteur;  // nbre de capteurs sur le drone
    T_AffInc mAffInc;   // affichage de l'incrustation
    T_MessInc mMessInc;  // contient les no de mesures à afficher et position
    CDeviceSpiMax7456 *mMax;

    void start();  // lance le timer d'incrustation
    void stop();  // efface les incrustation et stoppe le timer
    void razAff();  // vide le tableau de mesure à incruster
    void majAff();   // affiche l'incrustation
signals:

private slots:
    void onMessReady(long type);
    void onTimer();

public slots:

};

#endif // CINCRUSTER_H
