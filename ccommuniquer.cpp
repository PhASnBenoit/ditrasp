#include "ccommuniquer.h"

CCommuniquer::CCommuniquer(QObject *parent, CMsg *msg) :
    QObject(parent)
{
    pMsg = msg;
//    QList<QSerialPortInfo> listPsi;  // liste des ports série existant
//    listPsi = QSerialPortInfo::availablePorts();  // récupère les ports série disponibles

//    ps = new QSerialPort("/dev/ttyS0", this);
    qDebug("CCommuniquer démarre !");
}

void CCommuniquer::onMessReady(long type)
{
    int res;
    T_MessMes mess;
    if (type == 1) {
        qDebug("Message reçu dans pc_mesurer !!! Good !");
        res =  pMsg->getMessage(TYPE_MESS_ACK_ORDRE, &mess, sizeof(T_MessMes));
        if (res < 0)
           qDebug("Erreur extraction du message !");
        char ch[250];
        sprintf(ch,"Type=%ld     val=%d\n",mess.type, mess.corps.valInt);
        qDebug(ch);
    } // if
}
