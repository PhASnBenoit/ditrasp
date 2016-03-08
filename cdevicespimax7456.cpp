#include "cdevicespimax7456.h"

int CDeviceSpiMax7456::lireSpi(unsigned char *ch, int lg)
{
   int i;
   for(i=0 ; i<lg ; i++) {
       mSpi->lire1octet(ch++);
   } // for
   return 1;
}

int CDeviceSpiMax7456::init()
{
    unsigned char com[2];
    unsigned char ch;

    // init du composant d'incrustation
    com[0] = VM0; com[1]=0x48;  // PAL
    mSpi->ecrire(com,2);
    usleep(1000);
    com[0] = OSDBL|READ;  // pour lecture
    mSpi->ecrire(com,1);
    usleep(1000);
    mSpi->lire1octet(&ch);
    usleep(1000);
    com[1] = ch & 0xEF;  // mettre à 0 bit 4
    com[0] = OSDBL;
    mSpi->ecrire(com,2); // automatique black level
    usleep(1000);
    return 1;
}

CDeviceSpiMax7456::CDeviceSpiMax7456(QObject *parent, int noCe, int speed) :
    QObject(parent)
{
    mSpi = new CSpi(this, noCe, speed);
    this->init(); // init du composant MAX7456

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage de l'objet CDeviceSpiMax7456 !");
} // constructeur

CDeviceSpiMax7456::~CDeviceSpiMax7456()
{
    delete mSpi;
    delete mShm;
}

int CDeviceSpiMax7456::printRC(char *mes, int r, int c)
{
    const int cMax = 30;
    unsigned char reg=0;
    unsigned char com[2];

    // calcul de la position dans la display memory
    int dispMemAddr = r*cMax+c;
    (dispMemAddr>0xFF)?reg=0x01:reg=0x00;  // MSB à 1 si nécessaire
    com[0]=DMAH; com[1]=reg;
    mSpi->ecrire(com,2); // MSB de la position d'affichage

    com[0]=DMAL; com[1] = dispMemAddr&OCTET_BAS; // partie basse de l'adresse
    mSpi->ecrire(com,2); // Adresse de base d'affichage

    com[0]=DMM; com[1] = 0x41; // auto-increment et operation 8 bits
    mSpi->ecrire(com,2);

    com[0]=DMDI;
    for (size_t i=0 ; i<strlen(mes) ; i++) {
        com[1]=0x42; // caractère ? dans le référentiel MAX7456
        if (islower(*mes)) com[1]=*mes-0x3C; // adresse du car dans le référentiel MAX7456
        if (isupper(*mes)) com[1]=*mes-0x36;
        if (isdigit(*mes)) com[1]=*mes-0x30;
        if (*mes=='0') com[1]=0x0A;
        mSpi->ecrire(com,2);
        mes++; // car suivant
    } // for
    com[1]=0xFF;
    mSpi->ecrire(com,2);

    com[0]=DMM; com[1] = 0x00; // suppression auto increment et 8 bits.
    mSpi->ecrire(com,2);

    return -1;
} // printRC

int CDeviceSpiMax7456::effaceEcran()
{
    unsigned char reg=0;
    unsigned char com[2];
    com[1]=DMM|READ;
    mSpi->ecrire(com,1);  // demande de lecture de DMM
    mSpi->lire1octet(&reg);   // lecture de DMM
    com[0] = DMM;
    com[1] = reg | CLEAR_DISPLAY;
    mSpi->ecrire(com,2);
    usleep(20);  // temps d'effacement
    return 1;
} // destructeur
