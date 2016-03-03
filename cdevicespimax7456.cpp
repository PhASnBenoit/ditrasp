#include "cdevicespimax7456.h"

int CDeviceSpiMax7456::init()
{
    // init du composant d'incrustation
    return 1;
}

CDeviceSpiMax7456::CDeviceSpiMax7456(QObject *parent, int noCe, int speed) :
    QObject(parent)
{
    spi = new CSpi(this, noCe, speed);

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage du thread CCapteurLM76_Temp !");
} // constructeur

CDeviceSpiMax7456::~CDeviceSpiMax7456()
{

}

int CDeviceSpiMax7456::printXY(char *mes, int x, int y)
{
    return -1;
} // destructeur
