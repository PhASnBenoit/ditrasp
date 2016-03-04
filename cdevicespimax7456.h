#ifndef CDEVICESPIMAX7456_H
#define CDEVICESPIMAX7456_H


#include <QObject>
#include <string.h>
#include "global.h"
#include "cspi.h"

#define READ 0x80   // Faire un OU avec le reg pour lire
#define CLEAR_DISPLAY 4
#define OCTET_BAS 0xFF

enum regs_max7456 {
    VM0=0x00,   // write address
    VM1,   // write address
    HOS,   // write address
    VOS,   // write address
    DMM,   // write address
    DMAH,  // write address
    DMAL,  // write address
    DMDI,  // write address
    CMM,   // write address
    CMAH,  // write address
    CMAL,  // write address
    CMDI,  // write address
    OSDM,  // write address

    RB0=0x10,   // write address
    RB1,    // write address
    RB2,    // write address
    RB3,    // write address
    RB4,    // write address
    RB5,    // write address
    RB6,    // write address
    RB7,    // write address
    RB8,    // write address
    RB9,    // write address
    RB10,   // write address
    RB11,   // write address
    RB12,   // write address
    RB13,   // write address
    RB14,   // write address
    RB15,   // write address
    OSDBL=0x6C, // write address

    STAT=0xA0,  // read address only
    DMDO=0xB0,  // read address only
    CMDO=0xC0   // read address only
}; // enum

class CDeviceSpiMax7456 : public QObject
{
    Q_OBJECT

private:
    QSharedMemory *mShm;
    T_Mes *mData;   // pointeur du segment de mémoire partagé
    CSpi *mSpi;
    int init();

public:
    explicit CDeviceSpiMax7456(QObject *parent = 0, int noCe = '0', int speed = 250000);
    ~CDeviceSpiMax7456();
    int lireSpi(unsigned char *ch, int lg);  // pour les essais
    int printRC(char *mes, int r, int c);
    int effaceEcran();

signals:

public slots:

};

#endif // CDEVICEMAX7456_H
