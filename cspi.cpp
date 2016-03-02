#include "cspi.h"

CSpi::CSpi(QObject *parent, char noCe) :
    QObject(parent)
{
    mNoCe = noCe;
} // constructeur



int CSpi::lire(unsigned char *buffer, int lg)
{
    return read(mFileSpi, buffer, lg);
} // lire



int CSpi::ecrire(unsigned char *buffer, int lg)
{
    return write(mFileSpi, buffer, lg);
} // ecrire



int CSpi::init()
{
    if((mFileSpi=open("/dev/spidev1.1", O_RDWR))==-1) {  // ouvre le fichier virtuel d'accès à l'I2C
        qDebug("Erreur ouverture acces au bus SPI");
        return -1;
    } // if open

    return mFileSpi;
} // init
