#include "ccommuniquer.h"

CCommuniquer::CCommuniquer(QObject *parent, CMsg *msg) :
    QObject(parent)
{
    this->pParent = parent;
    pMsg = msg;
    // shm
    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug() << mShm->errorString();
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    QList<QSerialPortInfo> listPsi;  // liste des ports série existant
    listPsi = QSerialPortInfo::availablePorts();  // récupère les ports série disponibles
    qDebug() << listPsi.at(0).description();
    mPs = new QSerialPort(this);
    initPs(mPs);
    connect(mPs, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    // ouvrir le fichier de stockage des mesures
    QString nom("mesures.csv");
    mFileCsv = new QFile(nom);
    if (!mFileCsv->open(QIODevice::Truncate))
        qDebug() << "Erreur création fichier mesures.csv.";
    mFileCsv->close();
    mFileCsv->open(QIODevice::Append|QIODevice::Text);

    // init du timer de sauvegarde des mesures
    timer = new QTimer(this);
    timer->setInterval(2000);  // 2s par défaut
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    qDebug() << "CCommuniquer démarre !";
} // constructeur

CCommuniquer::~CCommuniquer()
{
    delete timer;
    mFileCsv->close();
    delete mFileCsv;
    mPs->close();
    delete mPs;
    mShm->detach();
    delete mShm;
}

int CCommuniquer::initPs(QSerialPort *mPs)
{
    return 1;
}  // destructeur

int CCommuniquer::protocole()
{
    QByteArray qba;
    QByteArray qbaCorps;
    QByteArray qbaTimer; // contient le nouvel interval

    qba = mPs->read(2);  // lecture de l'ordre de la GCS et du caractère ]
    if (qba.at(1) == ']') {  // si la syntaxe de fin est respectée
        switch (qba.at(0)) {
        case '0': // START MISSION
            break;
        case '1': // START ACQUISITION MESURE
            timer->start();
            break;
        case '2': // STOP ACQUISITION
            timer->stop();
            break;
        case '3': // RECEPTION PARAMS CONFIG MISSION
            break;
        case '4': // RECEPTION PARAMS INCRUSTATION
            break;
        case '5': // ORDRE CAMERA
            T_MessOrdre ordre;
            qbaCorps = mPs->read(35);  // requete GET de 35 caractères (mdp par défaut)
            strncpy(ordre.ordre, qbaCorps.toStdString().c_str(),35);
            pMsg->sendMessage(TYPE_MESS_ORDRE_CAMERA,&ordre, sizeof(T_MessOrdre));
            break;
        case '8': // CHANGE INTERVAL INCRUSTATION
            T_MessIntTimer it;
            qbaTimer = mPs->read(5);  // exemple format : "01000" pour 1s
            it.interval = qbaTimer.toInt();
            pMsg->sendMessage(TYPE_MESS_TIMERINC,&it, sizeof(T_MessIntTimer));
            break;
        case 'K': // Acquittement de la GCS
            break;
        default: // NON RECONNU
            qDebug() << "CCommuniquer: Ordre non reconnu";
            break;
        } // sw
    } else
        qDebug() << "CCommuniquer: Défaut fin ']' ordre !";

    return 1;
} // protocole

void CCommuniquer::onReadyRead()
{
    QByteArray qba=0;

    // recherche du premier caractère d'un ordre de la GCS
    while (mPs->bytesAvailable()) {
        qba = mPs->read(1);
        if (qba.at(0)=='[') {  // si premier caractère trouvé
            protocole();
        } // if [
    } // while
} // onReadyRead

void CCommuniquer::onTimer()
{
    // sauver les mesures dans le fichier sur une ligne horodatée
    QString ligne;
    ligne = QDateTime::currentDateTime().toString();
    ligne += ";";
    mShm->lock();
    for(int i=0 ; i<(mShm->size()/sizeof(T_Mes)) ; i++) {
        ligne += mData[i].valMes;
        ligne += ";";
    } // for
    mShm->unlock();
    mFileCsv->write(ligne.toStdString().c_str(), ligne.size());
    // emettre la mesure vers la GCS
    // étant dans le même objet, il ne peut y avoir de conflit sur la voie série

} // onTimer

void CCommuniquer::onMessReady(long type)
{
    int res;
    T_MessMes mess;
    if (type == 1) {
        qDebug() << "CCommuniquer: Message reçu !";
        res =  pMsg->getMessage(TYPE_MESS_ACK_ORDRE, &mess, sizeof(T_MessMes));
        if (res < 0)
           qDebug("Erreur extraction du message !");
        char ch[250];
        sprintf(ch,"Type=%ld     val=%d\n",mess.type, mess.corps.valInt);
        qDebug(ch);
    } // if
} // onMessReady
