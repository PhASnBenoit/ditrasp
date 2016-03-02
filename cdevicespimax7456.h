#ifndef CDEVICESPIMAX7456_H
#define CDEVICESPIMAX7456_H


#include <QObject>
#include "global.h"
#include "cspi.h"

class CDeviceSpiMax7456 : public QObject
{
    Q_OBJECT

private:
    QSharedMemory *mShm;
    T_Mes *mData;   // pointeur du segment de mémoire partagé
    float lireMesure();
    CSpi *spi;

public:
    explicit CDeviceSpiMax7456(QObject *parent = 0, int no = 0);

signals:

public slots:

};

#endif // CDEVICEMAX7456_H
