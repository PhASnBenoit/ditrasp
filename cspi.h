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
    // creation destruction de l'objet
    static CSpi *getInstance(QObject *parent = 0, char noCe = '0');
    static void freeInstance();
    int lire(unsigned char *buffer, int lg);
    int ecrire(unsigned char *buffer, int lg);
    int init();
    int getNbLink();

private:
    explicit CSpi(QObject *parent = 0, char noCe = 0);
    int mAddr;   // Adresse du composant SPI
    char mNoCe;   // No du device CE0 ou CE1
    int mFileSpi;  // descripteur du fichier i2C
    int mNbLink;
    static CSpi *mSingleton;
};

#endif // CSPI_H
