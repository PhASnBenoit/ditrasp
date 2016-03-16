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
    float angle;

    arret=false;
    while(!arret) {
        // écriture de la mesure dans le segment de mémoire partagé
        lireMesure(angle); // conversions incluses
        char chMes[15];
        sprintf(chMes,"%3.1f",angle);
        qDebug() << "CCapteurI2cHmc5883_Comp angle : " << angle;
        mShm->lock(); // on réserve la mémoire partagée
        strcpy(mData[mNum].valMes,chMes);  // écriture dans la mémoire partagée
        mShm->unlock(); // on libère la mémmoire partagée
        usleep(250000); // lecture toutes les 250ms
    } // while
}

void CCapteurI2cHmc5883_Comp::stop()
{
    arret=true;
} // run

////////////////////////////////////////////////////////////////////////////////////////////////////////
int CCapteurI2cHmc5883_Comp::lireMesure(float &angle)
{
    int res=0;
    double declin;
    unsigned short axes[3];
    double daxes[3];

    for (int i=0 ; i<3 ; i++) {
        res += i2c->lire(mAddrR, (unsigned char *)&axes[i], 2);  // valeur en micro tesla
        daxes[i] = axes[i];
    } // for
    // calcul de l'angle de déclinaison
    declin = atan2(daxes[1], daxes[0]); // résultat en radian
    if(declin < 0)
        declin += 2*PI;
    // Check for wrap due to addition of declination.
    if(declin > 2*PI)
        declin -= 2*PI;
    angle = (float)(declin*180/PI);
    return res;
} // lireCapteur
