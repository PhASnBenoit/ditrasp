#include "cmsg.h"

CMsg::CMsg(QObject *parent) :
    QObject(parent)
{
    membInited=false;
} // constructeur

CMsg::~CMsg()
{
} // destructeur

int CMsg::initialiser(const char *nomFic, int id)
{
    strncpy(membNomFic, nomFic, 254);
    membId = id;
    membClef = ftok(membNomFic, membId);  // calcul de la clef d'accès
    membMsQId = msgget(membClef, IPC_CREAT|0660);
    if (membMsQId==-1) return membMsQId;
    membInited = true;
    return membMsQId;
} // initialiser

int CMsg::sendMessage(long type, const void *mess, size_t taille)
{
    T_MessInc *pMess = (T_MessInc *) mess;
    if (!membInited) return -1;  // si non initialisation de la file
    pMess->type = type;
    //membMess = mess;
    int res = msgsnd(membMsQId, mess, taille, IPC_NOWAIT);  // retourne 0 si RAS
    if (!res) // si RAS
        emit mailReady(type);
    return res;
} // sendMessage

int CMsg::getMessage(long type, void *mess, size_t taille)
{
    if (!membInited) return -1;  // si non initialisation de la file
    return msgrcv(membMsQId, mess, taille, type, IPC_NOWAIT);
} // getMessage

int CMsg::detruire()
{
    membInited=false;
    return msgctl(membMsQId, IPC_RMID, 0); // effacement immédiat de la file
} // getMessage
