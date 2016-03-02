#include "cspi.h"

CSpi::CSpi(QObject *parent, char noCe) :
    QObject(parent)
{
    mNoCe = noCe;
    mNbLink = 0;
} // constructeur

CSpi * CSpi::mSingleton = NULL;

CSpi *CSpi::getInstance(QObject *parent, char noCe)
{
    if (mSingleton == NULL)
    {
        qDebug("L'objet CSpi sera créé !");
        mSingleton =  new CSpi(parent, noCe);
        mSingleton->mNbLink++;
    }
    else
    {
        qDebug("singleton already created!");
    }
    return mSingleton;
} // getInstance

void CSpi::freeInstance()
{
    if (mSingleton != NULL)
      {
            mSingleton->mNbLink--;
            if (mSingleton->mNbLink==0) {
                delete mSingleton;
                mSingleton = NULL;
            } // if mNbLink
      } // if null
} // freeInstance

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

int CSpi::getNbLink()
{
    return mNbLink;
} // getNbLink
