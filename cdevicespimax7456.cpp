#include "cdevicespimax7456.h"

CDeviceSpiMax7456::CDeviceSpiMax7456(QObject *parent, int noCe) :
    QObject(parent)
{
    spi = new CSpi(this, noCe, 250000);

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage du thread CCapteurLM76_Temp !");
}
