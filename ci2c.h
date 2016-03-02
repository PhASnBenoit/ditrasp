#ifndef CI2C_H
#define CI2C_H

#include <QObject>
#include <stdio.h>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

class CI2c : public QObject
{
    Q_OBJECT

public:
    // creation destruction de l'objet
    static CI2c *getInstance(QObject *parent = 0, char no = '1');
    static void freeInstance();
    int lire(unsigned char addr, unsigned char *buffer, int lg);
    int ecrire(unsigned char addr, unsigned char *buffer, int lg);
    int init();
    int getNbLink();

private:
    explicit CI2c(QObject *parent = 0, char noBus = '1');
    int mAddr;   // Adresse du composant I2C
    char mNoBus;   // No d'accès au fichier /dev
    int mFileI2c;  // descripteur du fichier i2C
    int mNbLink;
    static CI2c *mSingleton;
};

#endif // CI2C_H
