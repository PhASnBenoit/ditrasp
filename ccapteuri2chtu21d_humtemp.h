#ifndef CCAPTEURI2CHTU21D_HUMTEMP_H
#define CCAPTEURI2CHTU21D_HUMTEMP_H

#include <QThread>
#include <QDebug>
#include <stdio.h>
#include "global.h"
#include "ci2c.h"

class CCapteurI2cHtu21d_HumTemp : public QThread
{
    Q_OBJECT

private:
    int mNum;
    QSharedMemory *mShm;
    T_Mes *mData;   // pointeur du segment de mémoire partagé
    CI2c *mI2c;
    unsigned char mAddr;
    bool mArret;
    bool mStopped;
    float lireMesureHum();
    float lireMesureTemp();

public:
    explicit CCapteurI2cHtu21d_HumTemp(QObject *parent=0, int no=0, unsigned char addr=0x40);
    ~CCapteurI2cHtu21d_HumTemp();
    void run();  // méthode virtuelle à implémenter, contenu du thread

signals:

public slots:
    void stop();

};

#endif // CCAPTEURI2CHTU21D_HUMTEMP_H
