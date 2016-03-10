#ifndef CCAPTEURSERIALGPS_H
#define CCAPTEURSERIALGPS_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QByteArray>
#include <stdio.h>
#include "global.h"
#include "cserialport.h"

class CCapteurSerialGps : public QThread
{
    Q_OBJECT

public:
    explicit CCapteurSerialGps(QObject *parent = 0, const char *nomvs = "/dev/ttyAMA0", int no = 0, int vit = 9600, char par = 'N', int nbc = 8);
    ~CCapteurSerialGps();
    void run();  // méthode virtuelle à implémenter, contenu du thread

private:
    int mNum; // numéro de la mesure
    QSharedMemory *mShm;  // mémoire partagée
    T_Mes *mData;   // pointeur du segment de mémoire partagé
    int lireMesure(QByteArray &latitude, QByteArray &longitude);  // retourne lat et long
    CSerialPort *mPs;  // gestion du port série

signals:

public slots:

};

#endif // CCAPTEURSERIALGPS_H
