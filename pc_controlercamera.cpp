#include "pc_controlercamera.h"

PC_ControlerCamera::PC_ControlerCamera(QObject *parent, CMsg *msg) :
    QObject(parent)
{
    pMsg = msg;
    qDebug("PC_ControlerCamera démarre !");
}

void PC_ControlerCamera::onMessReady(long type)
{
    int res;
    T_MessMes mess;
    if (type == 3) {
        qDebug("Message reçu dans pc_mesurer !!! Good !");
        res =  pMsg->getMessage(1L, &mess, sizeof(T_MessMes));
        if (res < 0)
           qDebug("Erreur extraction du message !");
        char ch[250];
        sprintf(ch,"Type:%ld     val=%d\n",mess.type, mess.corps.valInt);
        qDebug(ch);
    } // if
}
