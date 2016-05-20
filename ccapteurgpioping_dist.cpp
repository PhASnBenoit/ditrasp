#include "ccapteurgpioping_dist.h"

CCapteurGpioPing_Dist::CCapteurGpioPing_Dist(QObject *parent, int no, unsigned char noGpio):
    QThread(parent)
{

    mNum = no;  // numéro de la mesure du fichier config.ini
    mNoGpio = noGpio;
    mGpio = new CGpio(mNoGpio);

    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    qDebug("Démarrage du thread CCapteurGpioPing_Dist !");

} // constructeur

CCapteurGpioPing_Dist::~CCapteurGpioPing_Dist()
{
    delete mGpio;
    delete mShm;
} // destructeur

void CCapteurGpioPing_Dist::run()
{
    int dist;
    mArret=false;
    mStopped=false;
    while(!mArret) {
        dist = getDistance();
        char chMes[30];
        sprintf(chMes,"Dist:%d",dist);
        mShm->lock(); // on prend la mémoire partagée
        strcpy(mData[mNum].valMes,chMes);  // écriture dans la mémoire partagée
        mShm->unlock(); // on libère la mémmoire partagée
    } // while
} // run

void CCapteurGpioPing_Dist::stop()
{
    mArret=true;
    while (!mStopped);
} // stop

int CCapteurGpioPing_Dist::getDistance()
{
    clock_t t1,t2;
    // Attendre avant la prochaine mesure
    mGpio->ecrire(0);
    usleep(500);
    // impulsion de commande
    mGpio->ecrire(1);
    usleep(10);
    mGpio->ecrire(0);

    // Attendre le passage a 1 de l'echo
    while(mGpio->lire()==0) ;
    t1=clock();

    // Attendre le retour à 0 de l'echo
    while(mGpio->lire()==1) ;
    t2=clock();

    // Calclu de la duree aller-retour
    unsigned long duration = (((unsigned long)t2)-((unsigned long)t1));
    // Calcul de la distance
    float distance=duration*34/2000;

    return distance;
} // getdistance
