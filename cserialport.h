#ifndef CSERIALPORT_H
#define CSERIALPORT_H

#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QFile>
#include <stdio.h>

class CSerialPort : public QObject
{
    Q_OBJECT

public:
    explicit CSerialPort(QObject *parent = 0, const char *nomvs = "/dev/ttyAMA0", int vit = 9600, char par = 'N', int nbc = 8);
    ~CSerialPort();
    int lireLigne(QByteArray &qba);

private:
    int mVit;  // vitesse
    char mPar;  // parité N:none E:even O:odd
    int mNbc;  // nb de bit par caractère
    char mNomVs[200];  // nom du fichier voie série
    QFile *mFile;
    int initPS();
signals:

public slots:

};

#endif // CSERIALPORT_H
