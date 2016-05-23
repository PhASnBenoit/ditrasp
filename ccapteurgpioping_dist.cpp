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
    qDebug() << CLOCKS_PER_SEC;

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
	clock_t t0,t1,t2;
    // Attendre avant la prochaine mesure
    mGpio->ecrire(0);
    usleep(500);
    // impulsion de commande
    mGpio->ecrire(1);
    usleep(10);
    mGpio->ecrire(0);
	
    // Attendre le passage a 1 de l'echo
	t0=clock();
    while(mGpio->lire()==0)
	{
/*		// Pas de passage à 1 avant le timeout
        if( (clock()-t0) > (TIMEOUT*CLOCKS_PER_SEC) )
            return -1;
*/	}
    t1=clock();

    // Attendre le retour à 0 de l'echo ou la fin du timeout
    while( (mGpio->lire()==1) /*&& ((clock()-t1)<(TIMEOUT*CLOCKS_PER_SEC))*/ );
    t2=clock();

    // Calclu de la duree aller-retour (CLOCKS_PER_SEC : nb de tick/s)
    unsigned long duration = (((unsigned long)t2)-((unsigned long)t1))/CLOCKS_PER_SEC;
    // Calcul de la distance (340 m/s trajet aller-retour)
    float distance=duration*34000/2;

    return distance;
} // getdistance
