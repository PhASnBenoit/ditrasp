#include "ccapteuri2clm76_temp.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
CCapteurI2cLm76_Temp::CCapteurI2cLm76_Temp(QObject *parent, int no) :
    QThread(parent)
{
    int res;

    mNum = no;  // numéro de la mesure du fichier config.ini

    unsigned char buf=0;
    i2c = CI2c::getInstance(this, '1', 0x48);  // N° du fichier virtuel et adr du composant I2C
    res = i2c->init();
    if (res == -1)
        qDebug("Pb init I2C");
    res = i2c->ecrire(&buf, 1);
    if (res != 1) qDebug("pb ecriture");

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage du thread CCapteurLM76_Temp !");
}  // constructeur

///////////////////////////////////////////////////////////////////////////////////////////////////////
CCapteurI2cLm76_Temp::~CCapteurI2cLm76_Temp()
{
    mShm->detach();
    delete mShm;
} // destructeur

void CCapteurI2cLm76_Temp::run()
{
    float mesure;
    while(1) {
        // écriture de la mesure dans le segment de mémoire partagé
        mesure = lireMesure();
        char chMes[10];
        sprintf(chMes,"%3.1f",mesure);
        mShm->lock(); // on prend la mémoire partagée
        strcpy(mData[mNum].valMes,chMes);  // écriture dans la mémoire partagée
        mShm->unlock(); // on libère la mémmoire partagée
        sleep(1);
    } // while
} // run

////////////////////////////////////////////////////////////////////////////////////////////////////////
float CCapteurI2cLm76_Temp::lireMesure()
{
    float temp;
    unsigned char mes[2];
    char aff[10];
    int res;

    res = i2c->lire(mes, 2);

    unsigned char msb = mes[0];
    unsigned char lsb = mes[1];
    sprintf(aff,"res=%d %02X %02X",res, msb, lsb);
    qDebug(aff);


    temp = ((((msb&0x7F)<<8) | lsb) >> 3)*0.0625;  // conversion
    if(msb&0x80) temp-= temp;  // si signe négatif
    return temp;
} // lireCapteur
