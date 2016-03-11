#include "cdevicespimax7456.h"

CDeviceSpiMax7456::CDeviceSpiMax7456(QObject *parent, int noCe, int speed) :
    QObject(parent)
{
    mSpi = new CSpi(this, noCe, speed);
    this->init(); // init du composant MAX7456
    qDebug("Démarrage de l'objet CDeviceSpiMax7456 !");
} // constructeur

CDeviceSpiMax7456::~CDeviceSpiMax7456()
{
    delete mSpi;
} // destructeur


int CDeviceSpiMax7456::lireSpi(unsigned char *ch, int lg)
{
   int i;
   for(i=0 ; i<lg ; i++) {
       mSpi->lire1octet(ch++);
   } // for
   return 1;
} // lireSpi

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
} // init

int CDeviceSpiMax7456::printRC(char *mes, int r, int c)
{
    const int cMax = 30;
    unsigned char reg=0;
    unsigned char com[2];

    com[0] = DMM; com[1]=0x00; // 16 bits
    mSpi->ecrire(com,2);
    usleep(1000);

    int dispMemAddr = r*cMax+c;
  //  qDebug() << "CDeviceSpiMax7456 : " << dispMemAddr << "mes=" << mes << "strlen=" << strlen(mes);

    while(*mes!=0) {  // on effectue la boucle jusqu'à la fin de la chaine (caractère NULL)
        (dispMemAddr>0xFF)?reg=0x01:reg=0x00;  // MSB à 1 si nécessaire
        com[0]=DMAH; com[1]=reg;
        mSpi->ecrire(com,2); // MSB de la position d'affichage
        usleep(1000);

        com[0]=DMAL; com[1] = dispMemAddr&OCTET_BAS; // partie basse de l'adresse
        mSpi->ecrire(com,2); // Adresse de base d'affichage
        usleep(1000);

        com[0]=DMDI; com[1]=0x42; // caractère ? dans le référentiel MAX7456
        if (islower(*mes)) com[1]=*mes-0x3C; // adresse du car dans le référentiel MAX7456
        if (isupper(*mes)) com[1]=*mes-0x36;
        if (isdigit(*mes)) com[1]=*mes-0x30;
        switch(*mes) {
        case '0': com[1]=0x0A; break;
        case ' ': com[1]=0x00; break;
        case '.': com[1]=0x41; break;
        case '(': com[1]=0x3F; break;
        case ')': com[1]=0x40; break;
        case ';': com[1]=0x43; break;
        case ':': com[1]=0x44; break;
        case ',': com[1]=0x45; break;
        case '\'':com[1]=0x46; break;
        case '/': com[1]=0x47; break;
        case '\"': com[1]=0x48; break;
        case '-': com[1]=0x49; break;
        case '<': com[1]=0x4A; break;
        case '>': com[1]=0x4B; break;
        case '@': com[1]=0x4C; break;
        }; // sw
        mSpi->ecrire(com,2);
        usleep(1000);
//        qDebug() << "caractère : " << *mes;
        mes++; // car suivant
        dispMemAddr++;
//        qDebug() << "Après inc : " << dispMemAddr;
    } // while

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
} // effaceEcran
