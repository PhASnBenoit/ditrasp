#include "ccapteuri2chmc5883_comp.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
CCapteurI2cHmc5883_Comp::CCapteurI2cHmc5883_Comp(QObject *parent, int no, unsigned char addr) :
    QThread(parent)
{
    int res;

    mNum = no;  // numéro de la mesure du fichier config.ini
    mAddrW = addr;  // Adresse du module I2C en écriture
    mAddrR = mAddrW;  // adresse pour la lecture

    mI2c = CI2c::getInstance(this, '1');  // N° du fichier virtuel et adr du composant I2C
    if (mI2c == NULL)
        qDebug("CCapteurI2cHmc5883_Comp: Pb init I2C");
    else qDebug() << "CCapteurI2cHmc5883_Comp: adresse " << mAddrW;
    // init du composant I2C
    unsigned char buf[]={0x00, // n° du registre config regA
                          0x70, // valeur Config Reg A
                          0xA0, // Config Reg B
                          0x00  // mode register
                         };
    res = mI2c->ecrire(mAddrW, buf, 4);
    usleep(200000);
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
    mI2c->freeInstance();
    mShm->detach();
    delete mShm;
} // destructeur

void CCapteurI2cHmc5883_Comp::run()
{
    short inclinx, incliny, inclinz;
    float declinz;

    mArret=false;
    mStopped=false;
    while(!mArret) {
        // écriture de la mesure dans le segment de mémoire partagé
        lireMesure(declinz, inclinx, incliny, inclinz); // conversions incluses
        char chMes[50];
        sprintf(chMes,"Angle:%3.1f",declinz);
//        qDebug() << "CCapteurI2cHmc5883_Comp, run angle : " << chMes;
        mShm->lock(); // on réserve la mémoire partagée
        strcpy(mData[mNum].valMes,chMes);  // écriture dans la mémoire partagée
        mShm->unlock(); // on libère la mémmoire partagée
        usleep(250000);
    } // while
    mStopped=true;
    //exec();
}

void CCapteurI2cHmc5883_Comp::stop()
{
    mArret=true;
    while(!mStopped);
} // run

////////////////////////////////////////////////////////////////////////////////////////////////////////
int CCapteurI2cHmc5883_Comp::lireMesure(float &declinz, short &inclinx,short &incliny, short &inclinz)
{
    int res=0;
    double resarc;
    unsigned char axe[6];

    unsigned char buf=0x03; // n° du registre premier axe
    res = mI2c->ecrire(mAddrW, &buf, 1); // positionnement registre axe x msb
    usleep(70000);
    res = mI2c->lire(mAddrR, axe, 6);  // valeur en micro tesla
    inclinx = (axe[0]<<8) + axe[1];
    inclinz = (axe[2]<<8) + axe[3];
    incliny = (axe[4]<<8) + axe[5];

    // calcul de l'angle de déclinaison
    resarc = atan2(incliny,inclinx); // résultat en radian
    if(resarc < 0)
        resarc += 2*PI;
    if(resarc > 2*PI)
        resarc -= 2*PI;
    declinz = (float)(resarc*180/PI);
    return res;
} // lireCapteur
