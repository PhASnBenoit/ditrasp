#include "cspi.h"

CSpi::CSpi(QObject *parent, char noCs, int speed) :
    QObject(parent)
{
    mNoCe = noCs;   // chip select
    mSpeed = speed;
    init();
    qDebug() << "Démarrage de l'objet CSpi";
} // constructeur

/////////////////////////////////////////////////////////////////
int CSpi::lire1octet(unsigned char *buffer)
{
    return read(mFileSpi, buffer, 1);
} // lire

/////////////////////////////////////////////////////////////////
int CSpi::ecrire(unsigned char *buffer, int lg)
{
    return write(mFileSpi, buffer, lg);
} // ecrire

/////////////////////////////////////////////////////////////////
int CSpi::init()
{
    char filename[20];

    sprintf(filename, "/dev/spidev0.%c", mNoCe);

    mFileSpi=open(filename, O_RDWR);
    if(mFileSpi==-1) {  // ouvre le fichier virtuel d'accès à SPI
          qDebug("Erreur ouverture acces au bus SPI");
          return -1;
    } // if open
    if (ioctl(mFileSpi, SPI_IOC_WR_MAX_SPEED_HZ, & mSpeed) != 0)
    {
         qDebug("Erreur ouverture acces au bus SPI");
         return -1;
    } // if
    return mFileSpi;
} // init
