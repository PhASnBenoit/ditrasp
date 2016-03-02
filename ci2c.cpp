#include "ci2c.h"

CI2c::CI2c(QObject *parent, char noBus, int addr) :
    QObject(parent)
{
    mNoBus = noBus;
    mAddr = addr;
    mNbLink=0;
} // constructeur

CI2c * CI2c::mSingleton = NULL;

int CI2c::lire(unsigned char *buffer, int lg)
{
    return read(mFileI2c, buffer, lg);
} // lire

int CI2c::ecrire(unsigned char *buffer, int lg)
{
    return write(mFileI2c, buffer, lg);
} // ecrire

int CI2c::init()
{
    char filename[20];
    sprintf(filename, "/dev/i2c-%c",mNoBus);
    if((mFileI2c=open(filename, O_RDWR))==-1) {  // ouvre le fichier virtuel d'accès à l'I2C
        qDebug("Erreur ouverture acces au bus I2C");
        return -1;
    } // if open
    if(ioctl(mFileI2c, I2C_SLAVE, mAddr)!=0) {  // Règle le driver I2C sur l'adresse.
        qDebug("Erreur ioctl acces au bus I2C");
        return -1;
    } // if ioctl
    return mFileI2c;
} // init

int CI2c::getNbLink()
{
    return mNbLink;
} // getNbLink

CI2c *CI2c::getInstance(QObject *parent, char no, int addr)
{
    if (mSingleton == NULL)
    {
        qDebug("L'objet CI2c sera créé !");
        mSingleton =  new CI2c(parent, no, addr);
        mSingleton->mNbLink++;
    }
    else
    {
        qDebug("singleton already created!");
    }
    return mSingleton;
} // getInstance

void CI2c::freeInstance()
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
