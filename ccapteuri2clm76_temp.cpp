#include "ccapteuri2clm76_temp.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
CCapteurI2cLm76_Temp::CCapteurI2cLm76_Temp(QObject *parent, int no, unsigned char addr) :
    QThread(parent)
{
    int res;

    mNum = no;  // numéro de la mesure du fichier config.ini
    mAddr = addr;

    unsigned char buf=0;
    mI2c = CI2c::getInstance(this, no);  // N° du fichier virtuel
    if (mI2c == NULL)
        qDebug("CCapteurI2cLm76_Temp: Pb init I2C");
    res = mI2c->ecrire(mAddr, &buf, 1);
    if (res != 1) qDebug("CCapteurI2cLm76_Temp: pb ecriture");
    usleep(250000);

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage du thread CCapteurLM76_Temp !");
}  // constructeur

///////////////////////////////////////////////////////////////////////////////////////////////////////
CCapteurI2cLm76_Temp::~CCapteurI2cLm76_Temp()
{
    mI2c->freeInstance();
    mShm->detach();
    delete mShm;
} // destructeur

void CCapteurI2cLm76_Temp::run()
{
    float mesure;

    mArret=false;
    while(!mArret) {
        // écriture de la mesure dans le segment de mémoire partagé
        mesure = lireMesure();
        char chMes[15];
        sprintf(chMes,"%3.1f",mesure);
        mShm->lock(); // on prend la mémoire partagée
        T_Mes *mess = (T_Mes *)mData;
        while (mess->noMes != mNum) mess++;
        strcpy(mess->valMes,chMes);  // écriture dans la mémoire partagée
        mShm->unlock(); // on libère la mémmoire partagée
        sleep(1); // lecture toutes les s
    } // while
}

void CCapteurI2cLm76_Temp::stop()
{
    mArret=true;
} // run

////////////////////////////////////////////////////////////////////////////////////////////////////////
float CCapteurI2cLm76_Temp::lireMesure()
{
    float temp;
    unsigned char mes[2];
    char aff[50];
    int res;

    res = mI2c->lire(mAddr, mes, 2);
    if (res != 2)
        qDebug() << "CCapteurI2cLm76_Temp:lireMesure res=" << res;
    unsigned char msb = mes[0];
    unsigned char lsb = mes[1];
    sprintf(aff,"CCapteurI2cLm76_Temp res=%d msb=%02X lsb=%02X",res, msb, lsb);
    qDebug(aff);
    temp = ((((msb&0x7F)<<8) | lsb) >> 3)*0.0625;  // conversion
    if(msb&0x80) temp-= temp;  // si signe négatif
    return temp;
} // lireCapteur
