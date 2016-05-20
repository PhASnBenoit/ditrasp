#include "cincruster.h"

CIncruster::CIncruster(QObject *parent, CMsg *msg, int interval) :
    QObject(parent)
{
    qDebug("CIncruster démarre !");

    // instanciation du composant d'incrustation
    mMax = new CDeviceSpiMax7456(this, '1', 250000);

    // attachement au segment de mémoire partagé des mesures instantanées
    mNbCapteur=0;
    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mData = (T_Mes *)mShm->constData(); // obtient le pointeur sur la mémoire
    for(int i=0 ; i<NBMAXCAPT ; i++) {
        if (strlen(mData[i].nomClasse)>0) { // capteur existant
                mNbCapteur++;
                mAffInc.c[i].r = mData[i].posL;
                mAffInc.c[i].c = mData[i].posC;
                mMessInc.c[i] = mData[i].noMes; // autorisation d'incrustation
        } // if capteur existant
        else {
            mMessInc.c[i] = -1;
        } // else
    } // for
    qDebug() << "CIncruster: " << mNbCapteur << "capteurs vus.";

    // lien vers la file de messages
    mMsg =msg;

    // envoi du message pour mise à jour de l'affichage
    mMsg->sendMessage(TYPE_MESS_INCRUSTER, &mMessInc, sizeof(T_MessInc));  // engendre un signal
    qDebug("CIncruster: envoi d'un message!");

    // timer d'interval d'incrustation
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
    mTimer->setInterval(interval); // valeur par défaut

} // constructeur

CIncruster::~CIncruster()
{
   mTimer->stop();
   delete mTimer;
   delete mMax;
   mShm->detach();
   delete mShm;
} // destructeur

void CIncruster::start()
{
    // démarrage du raffraichissement des valeurs incrustées
    mTimer->start();

} // start

void CIncruster::stop()
{
    // démarrage du raffraichissement des valeurs incrustées
    mTimer->stop();
    mMax->effaceEcran(); // vide écran vidéo des incrustations
} // stop

void CIncruster::razAff()
{
    for (int i=0 ; i<NBMAXCAPT ; i++) {
        mMessInc.c[i] = -1;  // init de tout l'affichage à -1 donc pas d'affichage
    } // for
} // razAff

void CIncruster::majAff()
{
    int *pI = mMessInc.c;  // pointe sur les mesures autorisées
    T_Aff *pA = mAffInc.c;  // pointe sur la structure d'incrustation

    qDebug() << "CIncruster::majAff";
    // modifier les valeurs d'incrustation
    static bool flag=true;
    for (int i=0 ; i<NBMAXCAPT ; i++) { // pour tous les capteurs dans le message
        (flag==true?flag=false:flag=true);
        if (*pI != -1) {
            mShm->lock();
            strcpy(pA->texte, mData[i].valMes);  // accès à la mémoire partagée
            strcat(pA->texte, mData[i].symbUnit);
            if (flag)
                strcat(pA->texte, "     ");
            else
                strcat(pA->texte, "  @  ");
            mShm->unlock();
            qDebug() << "--CIncruster:majAff:pI=" << *pI << " pA=" << pA->texte << " noMes=" << mData[i].noMes;
            mMax->printRC(pA->texte, pA->r, pA->c);
        } // if *pI
        pA++;  // pointe sur le champs suivant de la structure d'affichage
        pI++;  // pointe sur l'int suivant dans la structure message
    } // for
} // majAff

void CIncruster::onMessReady(long type)
{
    int res;
    T_MessIntTimer mess;

    qDebug("CIncruster:onMessReady: Un message est arrivé");
    switch (type) {
    case TYPE_MESS_INCRUSTER:  // modification des paramètres à afficher
        qDebug("CIncruster:onMessReady: nouveaux params d'incrustation");
        res =  mMsg->getMessage(TYPE_MESS_INCRUSTER, &mMessInc, sizeof(T_MessInc));  // lecture du message arrivé
        if (res < 0)
           qDebug() << "CIncruster:onMessReady: Erreur extraction du message !";
        break;
    case TYPE_MESS_TIMERINC: // modif de l'interval de raffraichissement
        res =  mMsg->getMessage(TYPE_MESS_TIMERINC, &mess, sizeof(T_MessIntTimer));  // lecture du message arrivé
        qDebug() << "CIncruster:onMessReady: nouveaux params timer : " << mess.interval;
        stop();
        mTimer->setInterval(mess.interval);
        if (mess.enable)
            start();
        break;
    default:
        qDebug("CIncruster:onMessReady: Signal non destiné à cet objet.");
        break;
    } // sw
} // onMessReady

void CIncruster::onTimer()
{
    // lire dans la shm les mesures en fonction de ce qu'il faut incruster
    majAff();
} // onTimer
