#include "cmsg.h"

CMsg::CMsg(QObject *parent) :
    QObject(parent)
{
    mInited=false;
} // constructeur

CMsg::~CMsg()
{
    detruire();
} // destructeur

int CMsg::initialiser(const char *nomFic, int id)
{
    strncpy(mNomFic, nomFic, 254);
    mId = id;
    mClef = ftok(mNomFic, mId);  // calcul de la clef d'accès
    mMsgId = msgget(mClef, IPC_CREAT|0666);
    if (mMsgId==-1) return mMsgId;
    mInited = true;
    return mMsgId;
} // initialiser

int CMsg::sendMessage(long type, const void *mess, size_t taille)
{
    T_MessInc *pMess = (T_MessInc *) mess;
    if (!mInited) return -1;  // si non initialisation de la file
    pMess->type = type;
    //mMess = mess;
    int res = msgsnd(mMsgId, mess, taille, IPC_NOWAIT);  // retourne 0 si RAS
    if (!res) // si RAS
        emit mailReady(type);
    return res;
} // sendMessage

int CMsg::getMessage(long type, void *mess, size_t taille)
{
    if (!mInited) return -1;  // si non initialisation de la file
    return msgrcv(mMsgId, mess, taille, type, IPC_NOWAIT);
} // getMessage

int CMsg::detruire()
{
    mInited=false;
    return msgctl(mMsgId, IPC_RMID, 0); // effacement immédiat de la file
} // getMessage
