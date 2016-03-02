#ifndef CMSG_H
#define CMSG_H

#include <QObject>
#include <QString>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "global.h"

#define TYPE_MESS_MESURE 1  // type du message pour transmettre une mesure
#define TYPE_MESS_INCRUSTER 2 //      ""    "       changer l'incrustation
#define TYPE_MESS_TIMERINC 21     //    ""              changer l'interval du timer d'incrustation

// type de message pour une mesure
typedef union umess {
    double valDouble;
    int valInt;
    char valCh[15];
} U_Mess;
typedef struct {
    long type;
    int noMesure;
    U_Mess corps;
} T_MessMes;

// type de message pour définir les mesures à incruster
typedef struct {
    long type;
    int hg;  // haut gauche
    int hc;  // centre
    int hd;  // droit
    int mg;  // milieu
    int mc;
    int md;
    int bg;  // bas
    int bc;
    int bd;
} T_MessInc;

// type pour changer l'interval d'un timer
typedef struct {
    long type;
    int interval;
} T_MessIntTimer;

/////////////////////////////////////////////////////////////////////////////
class CMsg : public QObject
{
    Q_OBJECT

public:
    explicit CMsg(QObject *parent = 0);
    ~CMsg();
    int initialiser(const char *nomFic, int id);
    int sendMessage(long type, const void *mess, size_t taille);
    int getMessage(long type, void *mess, size_t taille);
    int detruire();

private:
    int membId;             // pour le calcul de la clef
    char membNomFic[255];   // nom du fichier présent pour le calcul de la clef
    bool membInited;        // état initialise ou non
    key_t membClef;         // clef d'accès à la file de message
    int membMsQId;          // identifiant de la file de message
    T_MessMes membMess;     // réceptacle d'un message en cours de traitement

signals:
    void mailReady(long type);   // un message est arrivé dans la file

public slots:

};

#endif // CMSG_H
