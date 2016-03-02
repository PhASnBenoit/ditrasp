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
    explicit CSpi(QObject *parent = 0, char noCe = 0);
    int lire(unsigned char *buffer, int lg);
    int ecrire(unsigned char *buffer, int lg);
    int init();

private:
    int mAddr;   // Adresse du composant I2C
    char mNoCe;   // No du device CE0 ou CE1
    int mFileSpi;  // descripteur du fichier i2C

signals:

public slots:

};

#endif // CSPI_H
