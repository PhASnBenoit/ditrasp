#include "cincruster.h"

CIncruster::CIncruster(QObject *parent, CMsg *msg, int interval) :
    QObject(parent)
{
    qDebug("CIncruster démarre !");

    // init des positions d'affichage
    // améliorer en mettant les positions dans un fichier
    mAffInc.hg.c=1; mAffInc.hg.r=1;  // ligne du haut
    mAffInc.hm.c=10; mAffInc.hm.r=1;
    mAffInc.hd.c=20; mAffInc.hd.r=1;

    mAffInc.mg.c=1; mAffInc.mg.r=8;    // ligne du milieu
    mAffInc.mm.c=10; mAffInc.mm.r=8;
    mAffInc.md.c=20; mAffInc.md.r=8;

    mAffInc.bg.c=1; mAffInc.bg.r=15;   // ligne du bas
    mAffInc.bm.c=10; mAffInc.bm.r=15;
    mAffInc.bd.c=20; mAffInc.bd.r=15;

    // instanciation du composant d'incrustation
    mMax = new CDeviceSpiMax7456(this, '1', 250000);

    // attachement au segment de mémoire partagé pour les mesures instantanées
    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug(mShm->errorString().toStdString().c_str());
    mNbCapteur=mShm->size()/sizeof(T_Mes);  // nombre de mesures disponibles sur le drone
    qDebug() << "CIncruster: " << mNbCapteur << "capteurs vus." << mShm->size() << "/" << sizeof(T_Mes);
    mData = (T_Mes *)mShm->constData(); // obtient le pointeur sur la mémoire

    // lien vers la file de messages
    mMsg =msg;

    // initialisation de l'incrustation
    razAff();
    // au départ, si pas de message, on construit un message demandant l'incrustation
    // de toutes les mesures des capteurs présents !!!
    int *pNoMes = &mMessInc.hg;  // pointe sur la première position dans la structure du message
    for(int i=0 ; i<mNbCapteur ; i++) {
        pNoMes[mData[i].noMes] = mData[i].noMes;
        qDebug() << "CIncruster: i=" << i << " MessInc=" << pNoMes[mData[i].noMes] << "noMes=" << mData[i].noMes;
    } // for

    // envoi du message pour mise à jour de l'affichage
    mMsg->sendMessage(TYPE_MESS_INCRUSTER, &mMessInc, sizeof(T_MessInc));  // engendre un signal
    qDebug("CIncruster: envoi d'un message!");

    // démarrage du raffraichissement des valeurs incrustées
    mTimer = new QTimer(this);
    mTimer->setInterval(interval);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
    mTimer->start();
} // constructeur

CIncruster::~CIncruster()
{
   mTimer->stop();
   delete mTimer;
   delete mMax;
   mShm->detach();
   delete mShm;
} // destructeur

void CIncruster::razAff()
{
    int *pNoMes = &mMessInc.hg;
    for (int i=0 ; i<NBMAXCAPT ; i++) {
        *pNoMes++=-1;  // init de tout l'affichage à -1 donc pas d'affichage
    } // for
} // razAff

void CIncruster::majAff()
{
    T_Mes *mess;
    int *pI = &mMessInc.hg;  // pointe sur les mesures autorisées
    T_Aff *pA = &mAffInc.hg;  // pointe sur la structure d'incrustation
    //mMax->effaceEcran();

    // modifier les valeurs d'incrustation
    for (int i=0 ; i<NBMAXCAPT ; i++) { // pour tous les capteurs dans le message
        if (*pI != -1) {
            mess = (T_Mes *)mData;
            while(mess->noMes != *pI) mess++;
            mShm->lock();
            strcpy(pA->texte, mess->valMes);  // accès à la mémoire partagée
            strcat(pA->texte, mess->symbUnit);
            strcat(pA->texte, "    ");
            mShm->unlock();
            qDebug() << "--CIncruster:majAff:pI=" << *pI << " pA=" << pA->texte << " mess.noMes=" << mess->noMes;
            mMax->printRC(pA->texte, pA->r, pA->c);
        } // if *pI
        pA++;  // pointe sur le champs suivant de la structure d'affichage
        pI++;  // pointe sur l'int suivant dans la structure message
    } // for
} // majAff

void CIncruster::onMessReady(long type)
{
    int res;

    qDebug("CIncruster:onMessReady: Un message est arrivé");
    switch (type) {
    case TYPE_MESS_INCRUSTER:  // modification des paramètres à afficher
        qDebug("CIncruster:onMessReady: nouveaux params d'incrustation");
        res =  mMsg->getMessage(TYPE_MESS_INCRUSTER, &mMessInc, sizeof(T_MessInc));  // lecture du message arrivé
        if (res < 0)
           qDebug() << "CIncruster:onMessReady: Erreur extraction du message !";
        break;
    case TYPE_MESS_TIMERINC: // modif de l'interval de raffraichissement
        qDebug("CIncruster:onMessReady: nouveaux params timer");
        T_MessIntTimer mess;
        res =  mMsg->getMessage(TYPE_MESS_TIMERINC, &mess, sizeof(T_MessIntTimer));  // lecture du message arrivé
        mTimer->stop();
        mTimer->setInterval(mess.interval);
        mTimer->start();
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
