#ifndef CMSG_H
#define CMSG_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "global.h"

#define TYPE_MESS_MESURE 1  // type du message pour transmettre une mesure
#define TYPE_MESS_INCRUSTER 2 //      ""    "       changer l'incrustation
#define TYPE_MESS_TIMERINC 21     //    ""              changer l'interval du timer d'incrustation
#define TYPE_MESS_ORDRE_CAMERA 5  // pour prendre une photo, start/stop REC vidéo
#define TYPE_MESS_ACK_ORDRE 11  // pour communiquer ACK des ordres
#define TYPE_MESS_STOP_THREAD  // pour arrêt des threads capteurs

// type de message pour une mesure
typedef union umess {
    double valDouble;
    int valInt;
    char valCh[AFFMAX];
} U_Mess;
typedef struct {
    long type;
    int noMesure;
    U_Mess corps;
} T_MessMes;

// type de message pour autoriser les mesures à incruster
// -1 si pas d'incrustation de la mesure
typedef struct {
    long type;
    int c[9];  // nO du capteur à incruster
} T_MessInc;

// type pour un ordre
typedef struct {
    long type;
    char ordre[255];
} T_MessOrdre;

// type pour changer l'interval d'un timer
typedef struct {
    long type;
    int interval;
    bool enable; // pour start ou stop timer
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
    int mId;             // pour le calcul de la clef
    char mNomFic[255];   // nom du fichier présent pour le calcul de la clef
    bool mInited;        // état initialise ou non
    key_t mClef;         // clef d'accès à la file de message
    int mMsgId;          // identifiant de la file de message
    T_MessMes mMess;     // réceptacle d'un message en cours de traitement

signals:
    void mailReady(long type);   // un message est arrivé dans la file

public slots:

};

#endif // CMSG_H
