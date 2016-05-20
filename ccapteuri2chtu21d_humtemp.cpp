#include "ccapteuri2chtu21d_humtemp.h"

CCapteurI2cHtu21d_HumTemp::CCapteurI2cHtu21d_HumTemp(QObject *parent, int no, unsigned char addr) :
    QThread(parent)
{
    int res;

    mNum = no;  // numéro de la mesure du fichier config.ini
    mAddr = addr;

    unsigned char init[] = {0xE6, 0x03};

    mI2c = CI2c::getInstance(this, '1');  // N° du fichier virtuel
    if (mI2c == NULL)
        qDebug("CCapteurI2cHtu21d_HumTemp: Pb init I2C");

    // INIT HTU21D A FAIRE
    res = mI2c->ecrire(mAddr, init, 2);

    if (res != 2) qDebug("CCapteurI2cHtu21d_HumTemp: pb ecriture");
    usleep(250000);

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage du thread CCapteurI2cHtu21d_HumTemp !");
}

CCapteurI2cHtu21d_HumTemp::~CCapteurI2cHtu21d_HumTemp()
{
    stop();
    mI2c->freeInstance();
    mShm->detach();
    delete mShm;
}

void CCapteurI2cHtu21d_HumTemp::run()
{
    float mesureHum, mesureTemp;

    mArret=false;
    mStopped=false;
    while(!mArret) {
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
    mStopped=true;
} // run

void CCapteurI2cHtu21d_HumTemp::stop()
{
    mArret=true;
    while (!mStopped);
}

float CCapteurI2cHtu21d_HumTemp::lireMesureHum()
{
    float hum;
    unsigned char lecture[2];
    unsigned char ecriture=0xE5;
    char aff[50];
    int res;

    mI2c->ecrire(mAddr, &ecriture, 1);
    usleep(100000);
    res=mI2c->lire(mAddr, lecture, 2);
    if (res != 2)
        qDebug() << "CCapteurI2cHtu21d_HumTemp:lireMesureHum res=" << res;
    unsigned char MSB = lecture[0];
    unsigned char LSB = lecture[1];
    hum = -6+125*(MSB*256+LSB)/65536;
    sprintf(aff,"CCapteurI2cHtu21d_HumTemp res=%d msb=%02X lsb=%02X Hum:%3.1f",res, MSB, LSB, hum);
//    qDebug() << aff;
    return hum;
} // lireMesHum

float CCapteurI2cHtu21d_HumTemp::lireMesureTemp()
{
    float temp;
    unsigned char lecture[2];
    unsigned char ecriture=0xE3;
    char aff[50];
    int res;

    mI2c->ecrire(mAddr, &ecriture, 1);
    usleep(100000);
    res=mI2c->lire(mAddr, lecture, 2);
    if (res != 2)
        qDebug() << "CCapteurI2cHtu21d_HumTemp:lireMesureTemp res=" << res;
    unsigned char MSB = lecture[0];
    unsigned char LSB = lecture[1];
    temp = -46.85+175.72*(MSB*256+LSB)/65536;
    sprintf(aff,"CCapteurI2cHtu21d_HumTemp res=%d msb=%02X lsb=%02X Temp:%3.1f",res, MSB, LSB, temp);
//    qDebug() << aff;
    return temp;
} // lire MesTemp
