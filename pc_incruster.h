#ifndef PC_INCRUSTER_H
#define PC_INCRUSTER_H

#include <QObject>
#include <QTimer>
#include <stdio.h>
#include "cmsg.h"
#include "global.h"

typedef struct {
    char texte[15]; // ce champs doit rester en premier !
    int c,r;  // column, row
} T_Aff;

typedef struct {
    T_Aff hg; // haut gauche
    T_Aff hm;  //    milieu
    T_Aff hd;  // droite
    T_Aff mg; // milieu
    T_Aff mm;
    T_Aff md;
    T_Aff bg; // bas
    T_Aff bm;
    T_Aff bd;
} T_AffInc;

class PC_Incruster : public QObject
{
    Q_OBJECT

public:
    explicit PC_Incruster(QObject *parent = 0, CMsg *msg = 0, int interval = 1000);
    ~PC_Incruster();

private:
    CMsg *mMsg;
    QTimer *mTimer;
    QSharedMemory *mShm;   // pointeur de gestion de l'objet mémoire partagé
    const T_Mes *mData;   // acces à la mémoire partagée
    int mNbCapteur;  // nbre de capteurs sur le drone
    T_AffInc mAffInc;   // affichage de l'incrustation
    T_MessInc mMessInc;  // contient les no de mesures à afficher et position

    void razAff();
    void majAff();
signals:

private slots:
    void onMessReady(long type);
    void onTimer();

public slots:

};

#endif // PC_INCRUSTER_H
