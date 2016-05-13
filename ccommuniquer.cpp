#include "ccommuniquer.h"

CCommuniquer::CCommuniquer(QObject *parent, CMsg *msg) :
    QObject(parent)
{
    this->mParent = parent;
    mMsg = msg;
    mEtat=AVANT_MISSION;
    mEtatData = RECH_DEBUT_TRAME;

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
    QString nom("/home/pi/mesures.csv");
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

int CCommuniquer::protocole(QByteArray &qbaTrame, int lgTrame)
{
    US crcRecu, crcCalc;  // stockage des CRC recu et calculé
    T_MessIntTimer it;
    int lgCorps = lgTrame-9;

    qDebug() << "CCommuniquer::protocole: qba=" << qbaTrame;
    if (qbaTrame.at(3) == ']') {  // si la syntaxe de fin est respectée
        switch (qbaTrame.at(1)) {
        case '0':                   // PREMIER DIGIT A 0
            switch (qbaTrame.at(2)) { // deuxiéme digit
            case '0':               // START MISSION
                emit afficherTexte("START MISSION");
                if (mEtat != PENDANT_MISSION)
                    mEtat = PENDANT_MISSION;
                break;
            case '1': // START ACQUISITION MESURE ET INCRUSTATION ET ENVOI
                if (mEtat != PENDANT_MISSION)
                    return -1;
                emit afficherTexte("START ACQUISITION");
                crcRecu = retrouveCrc(qbaTrame.mid(lgTrame-4,4));  // reconstitue crc reçu sur 16 bits
                crcCalc = crc16((unsigned char *)qbaTrame.mid(4,lgCorps).toStdString().c_str(), lgCorps);
                qDebug() << "CCommuniquer::protocole: CRC Recu :" << crcRecu << " CRC Calc:" << crcCalc;
                if (crcCalc == crcRecu) {
                    // enregistrer nouveau timer
                    mTimerMesures = qbaTrame.mid(4, lgCorps).toInt();
                    // lance timer incrustation et envoi mesure si option choisie
                    mTimer->stop();   // normalement inutile
                    mTimer->setInterval(mTimerMesures);  // timer envoi mesures
//                  mTimer->start();  // pour le moment envoi obligatoire*/
                    // envoi message cincruster pour modif timer
                    it.interval = mTimerMesures;
                    it.enable=true;
                    mMsg->sendMessage(TYPE_MESS_TIMERINC,&it, sizeof(T_MessIntTimer));
                } else {    // else crc pas ok
                    qDebug() << "CCommuniquer::protocole: CRC Mauvais [1]";
                    return -1;
                } // else crc pas bon
                break;
            case '2': // STOP ACQUISITION
                if (mEtat != PENDANT_MISSION)
                    return -1;
                emit afficherTexte("STOP ACQUISITION");
                mTimer->stop();
                // message vers incrustation
                it.interval = mTimerMesures;
                it.enable=false;
                mMsg->sendMessage(TYPE_MESS_TIMERINC,&it, sizeof(T_MessIntTimer));
                break;
            case '3': // RECEPTION PARAMS CONFIG MISSION
                if (mEtat != AVANT_MISSION)
                    return -1;
                emit afficherTexte("PARAMS CONFIG AVANT MISSION");
                break;
            case '4': // RECEPTION PARAMS INCRUSTATION DEPART MISSION
                if (mEtat != AVANT_MISSION)
                    return -1;
                emit afficherTexte("PARAMS INCRUSTATION AVANT MISSION");
                break;
            case '5': // ORDRE CAMERA
                if (mEtat != PENDANT_MISSION)
                    return -1;
                emit afficherTexte("ORDRE CAMERA");
                crcRecu = retrouveCrc(qbaTrame.mid(lgTrame-4,4));  // reconstitue crc reçu sur 16 bits
                crcCalc = crc16((unsigned char *)qbaTrame.mid(4,lgCorps).toStdString().c_str(), lgCorps);
                qDebug() << "CCommuniquer::protocole: CRC Recu :" << crcRecu << " CRC Calc:" << crcCalc;
                if (crcCalc == crcRecu) {
                    // envoi message vers camera
                    T_MessOrdre ordre;
                    strncpy(ordre.ordre, qbaTrame.mid(4,lgCorps).toStdString().c_str(),lgCorps);
                    ordre.ordre[lgCorps]=0; // fin de chaine
                    mMsg->sendMessage(TYPE_MESS_ORDRE_CAMERA,&ordre, sizeof(T_MessOrdre));
                } else {    // if crc ok
                    qDebug() << "CCommuniquer::protocole: CRC Mauvais [5]";
                    return -1;
                } // else crc pas bon
                break;
            case '6':  // MODIF PARAMS INCRUSTE VIDEO
                if (mEtat != PENDANT_MISSION)
                    return -1;
                emit afficherTexte("MODIF PARAMS INCRUTATION");
                break;
            case '8': // CHANGE INTERVAL MISE A JOUR INCRUSTATION et TRANS MESURES
                if (mEtat != PENDANT_MISSION)
                    return -1;
                emit afficherTexte("MODIF INTERVAL INCRUSTATION");
                crcRecu = retrouveCrc(qbaTrame.mid(lgTrame-4,4));  // reconstitue crc reçu sur 16 bits
                crcCalc = crc16((unsigned char *)qbaTrame.mid(4,lgCorps).toStdString().c_str(), lgCorps);
                qDebug() << "CCommuniquer::protocole: CRC Recu :" << crcRecu << " CRC Calc:" << crcCalc;
                if (crcCalc == crcRecu) {
                    mTimerMesures = qbaTrame.mid(4,lgCorps).toInt();
                    it.interval = mTimerMesures;
                    it.enable=true;
                    mMsg->sendMessage(TYPE_MESS_TIMERINC,&it, sizeof(T_MessIntTimer));
                } else {    // if crc ok
                    qDebug() << "CCommuniquer::protocole: CRC Mauvais [8]";
                    return -1;
                } // else crc pas bon
                break;
            default: // Réception inconnue
                qDebug() << "CCommuniquer:protocole: Réception d'un ordre inconnu";
                return -1;
            } // sw digit2
            break;
        case '9': // Ordre 99 STOP MISSION (un seul ordre commence par 9)
            if (mEtat != PENDANT_MISSION)
                mEtat = APRES_MISSION;
            emit afficherTexte("STOP MISSION");
            mTimer->stop(); // stoppe l'envoi des mesures
            // message vers incrustation
            it.interval = mTimerMesures;
            it.enable=false;    // stoppe l'incrustation
            mMsg->sendMessage(TYPE_MESS_TIMERINC,&it, sizeof(T_MessIntTimer));
            break;
        case 'A': // Acquittement de la GCS (Un seul ordre commence par A)
            emit afficherTexte("Acquittement !");
            // acquittement par la GCS
            return -1;   // évite l'ACK automatique
            break;
        case 'T': // TEST de COMMUNICATION AVEC GCS
            qDebug() << "TEST RECU";
            emit afficherTexte("TEST RECU");
            // Rien à faire, juste renvoyer ACK
            // quelque soit l'état on peut le faire.
            break;
        default: // NON RECONNU
            qDebug() << "CCommuniquer:protocole: Réception d'un ordre inconnu";
            return -1;
            break;
        } // sw
    } else {
        qDebug() << "CCommuniquer: Défaut fin ']' ordre !";
        return -1;
    } // else ]
    return 1;
} // protocole

void CCommuniquer::onReadyRead()
{
    qDebug() << "CCommuniquer::onReadyRead";
    QByteArray car,
               trame;
    while (mPs->bytesAvailable()) {
        car = mPs->read(1); // lecture d'un caractère
        switch (mEtatData) {
        case RECH_DEBUT_TRAME:
            if (car.at(0) == '[') {
                mEtatData = SAUVE_TRAME;
                trame.append('[');
            } // if car
            break;
        case SAUVE_TRAME:
            if (car.at(0) == CARFIN) {  // fin de trame
                int res=protocole(trame, trame.size());
                //envoi de l'acquittement si tout va bien
                if (res>0) {
                    mPs->write("[AA]");
                    qDebug() << "CCommuniquer::onReadyRead: ACK envoyé";
                } // if res
                mEtatData = RECH_DEBUT_TRAME;
            } // if CARFIN
            else
                trame.append(car);
            break;
        default:
            qDebug() << "CCommuniquer::onReadyRead: Pb état réception.";
        } // sw
    } // while bytes
} // onReadyRead

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

US CCommuniquer::retrouveCrc(QByteArray qbaCrc)
{
    return (unsigned short) qbaCrc.toInt(NULL, 16);
} // retrouveCrc

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
