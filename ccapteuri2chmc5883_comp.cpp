#include "ccapteuri2chmc5883_comp.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
CCapteurI2cHmc5883_Comp::CCapteurI2cHmc5883_Comp(QObject *parent, int no, unsigned char addr) :
    QThread(parent)
{
    int res;

    mNum = no;  // numéro de la mesure du fichier config.ini
    mAddrW = addr;  // Adresse du module I2C en écriture
    mAddrR = mAddrW;  // adresse pour la lecture

    i2c = CI2c::getInstance(this, '1');  // N° du fichier virtuel et adr du composant I2C
    if (i2c == NULL)
        qDebug("CCapteurI2cHmc5883_Comp: Pb init I2C");

    // init du composant I2C
    unsigned char buf[4]={0x00, // n° du registre config regA
                          0x10, // valeur Config Reg A
                          0x20, // Config Reg B
                          0x00  // mode register
                         };
    res = i2c->ecrire(mAddrW, buf, 4);
    qDebug() << "HMC5883: nb car ecrits : " << res;
    if (res == -1) qDebug("CCapteurI2cHmc5883_Comp: pb ecriture");

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage du thread CCapteurI2cHmc5883_Comp !");
}  // constructeur

///////////////////////////////////////////////////////////////////////////////////////////////////////
CCapteurI2cHmc5883_Comp::~CCapteurI2cHmc5883_Comp()
{
    stop();
    i2c->freeInstance();
    mShm->detach();
    delete mShm;
} // destructeur

void CCapteurI2cHmc5883_Comp::run()
{
    int axes[3];

    arret=false;
    while(!arret) {
        // écriture de la mesure dans le segment de mémoire partagé
        lireMesure(axes); // conversions incluses
        char chMes[15];
        sprintf(chMes,"%03d %03d %03d",axes[0], axes[1], axes[2]);
        mShm->lock(); // on réserve la mémoire partagée
        strcpy(mData[mNum].valMes,chMes);  // écriture dans la mémoire partagée
        mShm->unlock(); // on libère la mémmoire partagée
        usleep(500000); // lecture toutes les 0.5s
    } // while
}

void CCapteurI2cHmc5883_Comp::stop()
{
    arret=true;
} // run

////////////////////////////////////////////////////////////////////////////////////////////////////////
int CCapteurI2cHmc5883_Comp::lireMesure(int axes[3])
{
    int res;
    for (int i=0 ; i<3 ; i++) {
        res = i2c->lire(mAddrR, (unsigned char *)&axes[i], 2);
        if (axes[i]>0) axes[i] = (axes[i]*180)/2047; // conversion en degré -180 --> 0
        else axes[i] = (axes[i]*180)/2048;           // conversion en degré 0 --> +180
    } // for
    return res;
} // lireCapteur
