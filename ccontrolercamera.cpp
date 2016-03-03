#include "ccontrolercamera.h"

CControlerCamera::CControlerCamera(QObject *parent, CMsg *msg) :
    QObject(parent)
{
    // paramètre de connexion à la gopro
    mPort=80;
    mAdrIP = QHostAddress(QString("10.5.5.9"));
    mMdp = "goprohero";
    mSock = new QTcpSocket(this);
    connect(mSock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(mSock, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(mSock, SIGNAL(bytesWritten(qint64)), SLOT(onBytesWritten(quint64)));

    pMsg = msg;
    qDebug("CControlerCamera démarre !");
}

CControlerCamera::~CControlerCamera()
{
    //delete mAdrIP;
    delete mSock;
}
/*
int CControlerCamera::capture(int etat)
{
    return 1;

}

int CControlerCamera::changeMode(int mode)
{
    return 1;

}

int CControlerCamera::preview(int etat)
{
    return 1;

}

int CControlerCamera::orientation(int etat)
{
    return 1;

}

int CControlerCamera::resolutionVideo(int res)
{
    return 1;

}

int CControlerCamera::resolutionPhoto(int res)
{
    return 1;

}

int CControlerCamera::timer(int interv)
{
    return 1;

}

int CControlerCamera::localisation(int etat)
{
    return 1;

}

int CControlerCamera::turn(int etat)
{
    QString str;
    if (etat)
        str="http://"+mAdrIP.toString()+"bacpac/PW?t="+mMdp+"&p=%"+QString(ON);
    else
        str="http://"+mAdrIP.toString()+"bacpac/PW?t="+mMdp+"&p=%"+QString(OFF);
    mSock->connectToHost(mAdrIP,mPort);
    mSock->write(str.toStdString().c_str());
    return 1;
}
*/
void CControlerCamera::onMessReady(long type)
{
    int res;
    T_MessOrdre mess;
    switch (type) {
    case TYPE_MESS_ORDRE_CAMERA :
        res =  pMsg->getMessage(TYPE_MESS_ORDRE_CAMERA, &mess, sizeof(T_MessOrdre));
        if (res < 0)
           qDebug("Erreur extraction du message !");
        mSock->connectToHost(mAdrIP,mPort);
        mSock->write(mess.ordre);
        qDebug(mess.ordre);
        break;
    } // sw
} // onMessReady

void CControlerCamera::onReadyRead()
{
    QByteArray qba;
    qba = mSock->readAll(); // lecture de l'octet de réponse (=0)
    qDebug("Acquittement de l'ordre par la Gopro");
    mSock->disconnectFromHost();
} // onReadyRead

void CControlerCamera::onConnected()
{
    qDebug("Connected to the Gopro");
} // onConnected

void CControlerCamera::onDisconnected()
{
    qDebug("Disconnected from the Gopro");
} // onDisconnected

void CControlerCamera::onBytesWritten(quint64 nb)
{
    qDebug("L'ordre a été transmis à la Gopro");
} // onBytesWritten
