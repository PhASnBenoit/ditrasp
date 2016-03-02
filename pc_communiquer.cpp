#include "pc_communiquer.h"

PC_Communiquer::PC_Communiquer(QObject *parent, CMsg *msg) :
    QObject(parent)
{
    pMsg = msg;
//    QList<QSerialPortInfo> listPsi;  // liste des ports série existant
//    listPsi = QSerialPortInfo::availablePorts();  // récupère les ports série disponibles

//    ps = new QSerialPort("/dev/ttyS0", this);
    qDebug("PC_Communiquer démarre !");
}

void PC_Communiquer::onMessReady(long type)
{
    int res;
    T_MessMes mess;
    if (type == 1) {
        qDebug("Message reçu dans pc_mesurer !!! Good !");
        res =  pMsg->getMessage(1L, &mess, sizeof(T_MessMes));
        if (res < 0)
           qDebug("Erreur extraction du message !");
        char ch[250];
        sprintf(ch,"Type=%ld     val=%d\n",mess.type, mess.corps.valInt);
        qDebug(ch);
    } // if
}
