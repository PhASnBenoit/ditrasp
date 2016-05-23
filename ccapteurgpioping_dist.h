#ifndef CCAPTEURGPIOPING_DIST_H
#define CCAPTEURGPIOPING_DIST_H

#include <QThread>
#include <QDebug>
#include <time.h> // for clock
#include "global.h"
#include "cgpio.h"

// TIMOUT = 20ms
#define TIMEOUT 0.02 // timeout d'echo du capteur
//#define CLOCKS_PER_SEC 1000000L

class CCapteurGpioPing_Dist : public QThread
{
    Q_OBJECT

private:
    int mNum;
    QSharedMemory *mShm;
    T_Mes *mData;   // pointeur du segment de mémoire partagé
    CGpio *mGpio;
    unsigned char mNoGpio;
    bool mArret;
    bool mStopped;
    float lireMesure();
    int getDistance();

public:
    explicit CCapteurGpioPing_Dist(QObject *parent = 0, int no = 0, unsigned char noGpio = 0x48);
    ~CCapteurGpioPing_Dist();
    void run();  // méthode virtuelle à implémenter, contenu du thread

signals:

public slots:
    void stop();

};
#endif // CCAPTEURGPIOPING_DIST_H
