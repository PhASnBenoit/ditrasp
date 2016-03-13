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

    pMsg = msg;

    qDebug("CControlerCamera démarre !");
} // constructeur

CControlerCamera::~CControlerCamera()
{
    delete mSock;
} // destructeur

void CControlerCamera::onMessReady(long type)
{
    int res;
    T_MessOrdre mess;
    switch (type) {
    case TYPE_MESS_ORDRE_CAMERA:
        res =  pMsg->getMessage(TYPE_MESS_ORDRE_CAMERA, &mess, sizeof(T_MessOrdre));
        if (res < 0)
           qDebug("Erreur extraction du message !");
        mSock->connectToHost(mAdrIP,mPort);
        mSock->write(mess.ordre);
        //mSock->close();
        qDebug(mess.ordre);
        break;
    default:
        qDebug("CControlerCamera: Message non reconnu reçu !");
        break;
    } // sw
} // onMessReady

void CControlerCamera::onReadyRead()
{
    QByteArray qba;
    qba = mSock->readAll(); // lecture de l'octet de réponse (=0)
    qDebug("Acquittement de l'ordre par la Gopro");
    mSock->close();
} // onReadyRead

void CControlerCamera::onConnected()
{
    qDebug() << "Connected to the Gopro";
} // onConnected

void CControlerCamera::onDisconnected()
{
    qDebug("Disconnected from the Gopro");
} // onDisconnected

