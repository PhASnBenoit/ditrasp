#ifndef CCAPTEURI2CHMC5883_COMP_H
#define CCAPTEURI2CHMC5883_COMP_H

#include <QThread>
#include <QDebug>
#include <stdio.h>
#include "global.h"
#include "ci2c.h"

class CCapteurI2cHmc5883_Comp : public QThread
{
    Q_OBJECT

private :
    int mNum;
    QSharedMemory *mShm;
    T_Mes *mData;   // pointeur du segment de mémoire partagé
    int lireMesure(int axes[3]);
    CI2c *i2c;
    unsigned char mAddrW, mAddrR; // adr d'écriture et lecture

public:
    explicit CCapteurI2cHmc5883_Comp(QObject *parent = 0, int no = 0, unsigned char addr = 0x1E); // no de la mesure définit dans le fichier de config "config.ini"
    ~CCapteurI2cHmc5883_Comp();
    void run();  // méthode virtuelle à implémenter, contenu du thread

signals:

public slots:

};

#endif // CCAPTEURI2CHMC5883_COMP_H
