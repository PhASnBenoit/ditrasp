#include "ccapteuri2chmc5883_comp.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
CCapteurI2cHmc5883_Comp::CCapteurI2cHmc5883_Comp(QObject *parent, int no, unsigned char addr) :
    QThread(parent)
{
    int res;

    mNum = no;  // numéro de la mesure du fichier config.ini
    mAddrW = addr<<1;  // Adresse du module I2C en écriture
    mAddrR = mAddrW+1;  // adresse pour la lecture

    i2c = CI2c::getInstance(this, '1');  // N° du fichier virtuel et adr du composant I2C
    res = i2c->init();
    if (res == -1)
        qDebug("CCapteurI2cHmc5883_Comp: Pb init I2C");

    // init du composant I2C
    unsigned char buf[3]={0x10, // Config Reg A
                          0x20, // Config Reg B
                          0x01  // mode register
                         };
    res = i2c->ecrire(mAddrW, buf, 3);
    if (res != 1) qDebug("CCapteurI2cHmc5883_Comp: pb ecriture");

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage du thread CCapteurI2cHmc5883_Comp !");
}  // constructeur

///////////////////////////////////////////////////////////////////////////////////////////////////////
CCapteurI2cHmc5883_Comp::~CCapteurI2cHmc5883_Comp()
{
    i2c->freeInstance();
    mShm->detach();
    delete mShm;
} // destructeur

void CCapteurI2cHmc5883_Comp::run()
{
    int axes[3];
    while(1) {
        // écriture de la mesure dans le segment de mémoire partagé
        lireMesure(axes);
        char chMes[15];
        sprintf(chMes,"%03x %03x %03x",axes[0], axes[1], axes[2]);
        mShm->lock(); // on prend la mémoire partagée
        strcpy(mData[mNum].valMes,chMes);  // écriture dans la mémoire partagée
        mShm->unlock(); // on libère la mémmoire partagée
        sleep(1);
    } // while
} // run

////////////////////////////////////////////////////////////////////////////////////////////////////////
int CCapteurI2cHmc5883_Comp::lireMesure(int axes[3])
{
    int res;
    res = i2c->lire(mAddrR, (unsigned char *)&axes[0], 2);
    res = i2c->lire(mAddrR, (unsigned char *)&axes[1], 2);
    res = i2c->lire(mAddrR, (unsigned char *)&axes[2], 2);
    return res;
} // lireCapteur
