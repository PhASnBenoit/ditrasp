#include "ccapteurserialgps.h"

CCapteurSerialGps::CCapteurSerialGps(QObject *parent, const char *nomvs, int no, int vit, char par, int nbc) :
    QThread(parent)
{
    mNum = no;  // numéro de la mesure du fichier config.ini

    mPs = new CSerialPort(this, nomvs, vit, par, nbc);

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug() << mShm->errorString();
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug() << "Démarrage du thread CCapteurSerialGps !";
} // construteur

CCapteurSerialGps::~CCapteurSerialGps()
{
    delete mPs;
    mShm->detach();
    delete mShm;
} // destructeur

void CCapteurSerialGps::run()
{
    QByteArray latitude, longitude;
    QString mesure;

    while(1) {
        lireMesure(latitude, longitude);
        mesure = "Lat:"+QString(latitude)+" Long:"+QString(longitude);
        // sauvegarde dans la mémoire partagée
        mShm->lock();
          strncpy(mData[mNum].valMes, mesure.toStdString().c_str(), sizeof(mData[mNum].valMes));
        mShm->unlock();
    } // while
} // run

int CCapteurSerialGps::lireMesure(QByteArray &latitude, QByteArray &longitude)
{
    QByteArray ligne;
    QList<QByteArray> nmeaGGA;
    // lecture du GPS
    mPs->lireLigne(ligne);
    // décodage de lat et long
    nmeaGGA = ligne.split(',');
    latitude = nmeaGGA.at(2)+nmeaGGA.at(3);
    longitude = nmeaGGA.at(4)+nmeaGGA.at(5);
    return 1;
} // lireMesure
