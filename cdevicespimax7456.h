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
    int init();

public:
    explicit CDeviceSpiMax7456(QObject *parent = 0, int noCe = '0', int speed = 250000);
    ~CDeviceSpiMax7456();
    int printXY(char *mes, int x, int y);

signals:

public slots:

};

#endif // CDEVICEMAX7456_H
