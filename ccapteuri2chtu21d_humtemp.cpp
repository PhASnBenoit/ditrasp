#include "ccapteuri2chtu21d_humtemp.h"

CCapteurI2cHtu21d_HumTemp::CCapteurI2cHtu21d_HumTemp(QObject *parent, int no, unsigned char addr)
{
    int res;

    mNum = no;  // numéro de la mesure du fichier config.ini
    mAddr = addr;

    unsigned char buf=0;

    i2c = CI2c::getInstance(this, '1');  // N° du fichier virtuel
    if (i2c == NULL)
        qDebug("CCapteurI2cHtu21d_HumTemp: Pb init I2C");

    // INIT HTD21D A FAIRE
    res = i2c->ecrire(mAddr, &buf, 1);

    if (res != 1) qDebug("CCapteurI2cHtu21d_HumTemp: pb ecriture");
    usleep(250000);

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage du thread CCapteurI2cHtu21d_HumTemp !");
}

CCapteurI2cHtu21d_HumTemp::~CCapteurI2cHtu21d_HumTemp()
{
    i2c->freeInstance();
    mShm->detach();
    delete mShm;
}

void CCapteurI2cHtu21d_HumTemp::run()
{
    float mesureHum, mesureTemp;
    arret=false;

    while(!arret) {
        // écriture de la mesure dans le segment de mémoire partagé
        mesureHum = lireMesureHum();
        mesureTemp = lireMesureTemp();
        char chMes[30];
        sprintf(chMes,"Hum:%3.1f Temp:%3.1f",mesureHum, mesureTemp);
        mShm->lock(); // on prend la mémoire partagée
        strcpy(mData[mNum].valMes,chMes);  // écriture dans la mémoire partagée
        mShm->unlock(); // on libère la mémmoire partagée
        sleep(1); // lecture toutes les s
    } // while
}

void CCapteurI2cHtu21d_HumTemp::stop()
{
    arret=true;
}

float CCapteurI2cHtu21d_HumTemp::lireMesureHum()
{
    float hum;
    unsigned char mes[2];
    char aff[50];
    int res;

    res = i2c->lire(mAddr, mes, 2);
    if (res != 2)
        qDebug() << "CCapteurI2cHtu21d_HumTemp:lireMesureHum res=" << res;
    unsigned char msb = mes[0];
    unsigned char lsb = mes[1];
    sprintf(aff,"CCapteurI2cHtu21d_HumTemp res=%d msb=%02X lsb=%02X",res, msb, lsb);
    qDebug(aff);

    // CONVERSION A FAIRE
    hum = ((((msb&0x7F)<<8) | lsb) >> 3)*0.0625;  // conversion
    //if(msb&0x80) temp-= temp;  // si signe négatif
    return hum;
}

float CCapteurI2cHtu21d_HumTemp::lireMesureTemp()
{
    float temp;
    unsigned char mes[2];
    char aff[50];
    int res;

    res = i2c->lire(mAddr, mes, 2);
    if (res != 2)
        qDebug() << "CCapteurI2cHtu21d_HumTemp:lireMesureTemp res=" << res;
    unsigned char msb = mes[0];
    unsigned char lsb = mes[1];
    sprintf(aff,"CCapteurI2cHtu21d_HumTemp res=%d msb=%02X lsb=%02X",res, msb, lsb);
    qDebug(aff);
    temp = ((((msb&0x7F)<<8) | lsb) >> 3)*0.0625;  // conversion
    if(msb&0x80) temp-= temp;  // si signe négatif
    return temp;
}
