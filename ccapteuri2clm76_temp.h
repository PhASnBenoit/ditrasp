#ifndef CCAPTEURI2CLM76_TEMP_H
#define CCAPTEURI2CLM76_TEMP_H

#include <QThread>
#include <QDebug>
#include <stdio.h>
#include "global.h"
#include "ci2c.h"

class CCapteurI2cLm76_Temp : public QThread
{
    Q_OBJECT

private :
    int mNum;
    QSharedMemory *mShm;
    T_Mes *mData;   // pointeur du segment de mémoire partagé
    CI2c *mI2c;
    unsigned char mAddr;
    bool mArret;
    bool mStopped;
    float lireMesure();

public:
    explicit CCapteurI2cLm76_Temp(QObject *parent = 0, int no = 0, unsigned char addr = 0x48); // no de la mesure définit dans le fichier de config "config.ini"
    ~CCapteurI2cLm76_Temp();
    void run();  // méthode virtuelle à implémenter, contenu du thread


signals:

public slots:
    void stop();
};

#endif // CCAPTEURI2CLM76_TEMP_H
