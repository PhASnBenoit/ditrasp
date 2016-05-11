#include "cserialport.h"

CSerialPort::CSerialPort(QObject *parent, const char *nomvs, int vit, char par, int nbc) :
    QObject(parent)
{
    mVit = vit; mPar = par; mNbc = nbc;  // params voie série
    strncpy(mNomVs, nomvs, sizeof(mNomVs));

    mFile = new QFile(QString(nomvs));
    if (!mFile->open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << "Erreur ouverture port série";
    // ajouter l'init des params voie série
    initPS(); // à définir
} // constructeur

CSerialPort::~CSerialPort()
{
    mFile->close();
    delete mFile;
}

int CSerialPort::lireLigne(QByteArray &qba)
{
    qba = mFile->readLine();
    return 1;
}

int CSerialPort::initPS()
{

    return 1;
} // destructeur
