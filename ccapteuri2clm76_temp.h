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
    float lireMesure();
    CI2c *i2c;

public:
    explicit CCapteurI2cLm76_Temp(QObject *parent = 0, int no = 0); // no de la mesure définit dans le fichier de config "config.ini"
    ~CCapteurI2cLm76_Temp();
    void run();  // méthode virtuelle à implémenter, contenu du thread

signals:

public slots:

};

#endif // CCAPTEURI2CLM76_TEMP_H
