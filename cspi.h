#ifndef CSPI_H
#define CSPI_H

#include <QObject>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

class CSpi : public QObject
{
    Q_OBJECT

public:
    explicit CSpi(QObject *parent = 0, char noCs = '0', int speed=250000);
    int lire1octet(unsigned char *buffer);
    int ecrire(unsigned char *buffer, int lg);

private:
    int init();
    int mAddr;   // Adresse du composant SPI
    char mNoCe;   // No du device CE/
    int mSpeed;   // vitesse du bus SPI pour CE0
    int mFileSpi;  // descripteur du fichier Spi
};

#endif // CSPI_H
