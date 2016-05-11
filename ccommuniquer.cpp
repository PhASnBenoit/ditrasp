#include "ccommuniquer.h"

CCommuniquer::CCommuniquer(QObject *parent, CMsg *msg) :
    QObject(parent)
{
    this->mParent = parent;
    mMsg = msg;
    // shm
    mShm = new QSharedMemory(KEY, this);  // pointeur vers l'objet mémoire partagé
    if (!mShm->attach())
        qDebug() << mShm->errorString();
    mData = (T_Mes *)mShm->data(); // obtient le pointeur sur la mémoire

    QList<QSerialPortInfo> listPsi;  // liste des ports série existant
    listPsi = QSerialPortInfo::availablePorts();  // récupère les ports série disponibles
    for(int i=0 ; i<listPsi.size() ; i++)
        qDebug() << "CCommuniquer : " << listPsi.at(i).portName() << " " << listPsi.at(i).description();

    // ouverture de la liaison data
    mPs = new QSerialPort("ttyUSB0",this);
    initPs(mPs);
    connect(mPs, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    // ouvrir le fichier de stockage des mesures de la mission
    QString nom("~/mesures.csv");
    mFileCsv = new QFile(nom);
    if (!mFileCsv->open(QIODevice::Truncate))
        qDebug() << "CCommuniquer : " << "Erreur création fichier mesures.csv.";
    mFileCsv->close();
    mFileCsv->open(QIODevice::Append|QIODevice::Text);

    // init du timer de sauvegarde des mesures
    mTimerMesures = 2000;  // par défaut
    mTimer = new QTimer(this);
    mTimer->setInterval(mTimerMesures);  // 2s par défaut
    connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimer()));

    qDebug() << "CCommuniquer démarre !";
} // constructeur

CCommuniquer::~CCommuniquer()
{
    delete mTimer;
    mFileCsv->close();
    delete mFileCsv;
    mPs->close();
    delete mPs;
    mShm->detach();
    delete mShm;
}

int CCommuniquer::initPs(QSerialPort *serial)
{
    serial->setPortName("ttyUSB0");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        serial->setRequestToSend(false);
        qDebug() << "Connected to ttyUSB0";
        connect(serial, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    } else
        qDebug() << "Impossible to connect to ttyUSB0";
    return 1;
}  // destructeur

int CCommuniquer::protocole()
{
    US crcRecu, crcCalc;  // stockage des CRC recu et calculé
    QByteArray qba;
    QByteArray qbaCorps;
    QByteArray qbaCrc;

    qDebug() << "CCommuniquer::protocole: Protocole en route";

    qbaCrc.fill(0,2);
    // recherche du premier caractère d'un ordre de la GCS
    while (mPs->bytesAvailable()) {
        qba.resize(1);
        qba = mPs->read(1);
        if (qba.at(0)=='[') {  // si premier caractère trouvé
            qba.fill(0,2);
            qba = mPs->read(2);  // lecture de l'ordre de la GCS et du caractère ]
            if (qba.at(1) == ']') {  // si la syntaxe de fin est respectée
                switch (qba.at(0)) {
                case '0':                   // PREMIER DIGIT A 0
                    switch (qba.at(1)) {
                    case '0':               // START MISSION
                        break;
                    case '1': // START ACQUISITION MESURE ET INCRUSTATION ET ENVOI
                        qbaCorps.fill(0,5);
                        qbaCorps = mPs->read(5);  // Lecture des datas : 5 octets interval timer trans mesures
                        qbaCrc = mPs->read(2);  // lecture du CRC16
                        crcRecu = (qbaCrc.at(0)<<8)+qbaCrc.at(1);  // reconstitue crc sur 16 bits
                        crcCalc = crc16((unsigned char *)qbaCorps.toStdString().c_str(), 5);
                        qDebug() << "CRC Recu :" << crcRecu << " CRC Calc:" << crcCalc;
                        if (crcCalc == crcRecu) {
                            // enregistrer nouveau timer
                            mTimerMesures = qbaCorps.toInt();
                            // lance timer incrustation et envoi mesure si option choisie
                            mTimer->stop();   // normalement inutile
                            mTimer->setInterval(mTimerMesures);  // timer envoi mesures
//                            mTimer->start();  // pour le moment envoi obligatoire
                            // envoi message cincruster pour modif timer
                            T_MessIntTimer it;
                            it.interval = mTimerMesures;
                            it.enable=true;
                            mMsg->sendMessage(TYPE_MESS_TIMERINC,&it, sizeof(T_MessIntTimer));
                        } else {    // if crc ok
                            qDebug() << "CCommuniquer::protocole: CRC Mauvais [1]";
                        } // else crc pas bon
                        break;
                    case '2': // STOP ACQUISITION
                        mTimer->stop();
                        // message vers incrustation
                        T_MessIntTimer it;
                        it.interval = mTimerMesures;
                        it.enable=false;
                        mMsg->sendMessage(TYPE_MESS_TIMERINC,&it, sizeof(T_MessIntTimer));
                        break;
                    case '3': // RECEPTION PARAMS CONFIG MISSION
                        break;
                    case '4': // RECEPTION PARAMS INCRUSTATION DEPART MISSION
                        break;
                    case '5': // ORDRE CAMERA
                        qbaCorps.fill(0,35);
                        qbaCorps = mPs->read(35);  // requete fixe GET de 35 caractères (mdp par défaut)
                        qbaCrc = mPs->read(2);  // lecture du CRC16
                        crcRecu = (qbaCrc.at(0)<<8)+qbaCrc.at(1);  // reconstitue crc sur 16 bits
                        crcCalc = crc16((unsigned char *)qbaCorps.toStdString().c_str(), 35);
                        qDebug() << "CRC Recu :" << crcRecu << " CRC Calc:" << crcCalc;
                        if (crcCalc == crcRecu) {
                            // envoi message vers camera
                            T_MessOrdre ordre;
                            strncpy(ordre.ordre, qbaCorps.toStdString().c_str(),35);
                            mMsg->sendMessage(TYPE_MESS_ORDRE_CAMERA,&ordre, sizeof(T_MessOrdre));
                        } else {    // if crc ok
                            qDebug() << "CCommuniquer::protocole: CRC Mauvais [5]";
                        } // else crc pas bon
                        break;
                    case '6':  // MODIF PARAMS INCRUSTE VIDEO
                        break;
                    case '8': // CHANGE INTERVAL MISE A JOUR INCRUSTATION et TRANS MESURES
                        qbaCorps.fill(0,5);
                        qbaCorps = mPs->read(5);  // exemple format : "01000" pour 1s
                        qbaCrc = mPs->read(2);  // lecture du CRC16
                        crcRecu = (qbaCrc.at(0)<<8)+qbaCrc.at(1);  // reconstitue crc sur 16 bits
                        crcCalc = crc16((unsigned char *)qbaCorps.toStdString().c_str(), 5);
                        qDebug() << "CRC Recu :" << crcRecu << " CRC Calc:" << crcCalc;
                        if (crcCalc == crcRecu) {
                            mTimerMesures = qbaCorps.toInt();
                            T_MessIntTimer it;
                            it.interval = mTimerMesures;
                            it.enable=true;
                            mMsg->sendMessage(TYPE_MESS_TIMERINC,&it, sizeof(T_MessIntTimer));
                        } else {    // if crc ok
                            qDebug() << "CCommuniquer::protocole: CRC Mauvais [8]";
                        } // else crc pas bon
                        break;
                    default: // Réception inconnue
                        qDebug() << "CCommuniquer:protocole: Réception d'un ordre inconnu";
                        return -1;
                    } // sw digit2
                    break;
                case '9': // Ordre 99 STOP MISSION (un seul ordre commence par 9)
                    break;
                case 'A': // Acquittement de la GCS (Un seul ordre commence par A)
                    break;
                case 'T': // TEST de COMMUNICATION AVEC GCS
                    // Rien à faire, juste renvoyer ACK
                    break;
                default: // NON RECONNU
                    qDebug() << "CCommuniquer:protocole: Réception d'un ordre inconnu";
                    return -1;
                    break;
                } // sw
            } else {
                qDebug() << "CCommuniquer: Défaut fin ']' ordre !";
                return -1;
            } // else
        } // if [
    } // while
    return 1;
}

US CCommuniquer::crc16(UC *tab,int nb)
{
    UC nbDec,         // indique le nombre de décalage de l'octet */
       yaUn,          // booleen si bit = 1 alors =1
       ind;           // indique l'indice dans la chaine
    US crc;  // contient le crc16

    crc = 0xFFFF;
    ind = 0;

    do {
        crc ^= (US)tab[ind];
        nbDec = 0;
        do {
            if ((crc & 0x0001) == 1)
                yaUn = 1;
            else
                yaUn = 0;
            crc >>= 1;
            if (yaUn)
                crc ^= 0xA001;
            nbDec++;
        } while (nbDec < 8);
        ind++;
    } while (ind < nb);
    return(crc);
} // crc16

void CCommuniquer::onReadyRead()
{
  int res=protocole();
  //envoi de l'acquittement si tout va bien
  if (res>0) {
     mPs->write("[AA]");
     qDebug() << "CCommuniquer::onReadyRead: ACK envoyé";
  } // if res
} // onReadyRead

void CCommuniquer::onTimer()
{
    // sauver les mesures dans le fichier sur une ligne horodatée
    QString ligne;
    ligne = QDateTime::currentDateTime().toString();
    ligne += ";";
    mShm->lock();
    for(int i=0 ; i<(int)(mShm->size()/sizeof(T_Mes)) ; i++) {
        ligne += mData[i].valMes;
        ligne += ";";
    } // for
    mShm->unlock();
    // emettre la mesure vers la GCS
    // étant dans le même objet, il ne peut y avoir de conflit sur la voie série
    mFileCsv->write(ligne.toStdString().c_str(), ligne.size());

} // onTimer

void CCommuniquer::onMessReady(long type)
{
    int res;
    T_MessMes mess;
    if (type == TYPE_MESS_ACK_ORDRE) {
        qDebug() << "CCommuniquer: Message reçu !";
        res =  mMsg->getMessage(TYPE_MESS_ACK_ORDRE, &mess, sizeof(T_MessMes));
        if (res < 0)
           qDebug("Erreur extraction du message !");
        char ch[250];
        sprintf(ch,"Type=%ld     val=%d\n",mess.type, mess.corps.valInt);
        qDebug(ch);
    } // if
} // onMessReady
