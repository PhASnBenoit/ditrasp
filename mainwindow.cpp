#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // lecture du fichier de configuration config.ini qui identifie les capteurs présents sur le drone
    QList<T_Mes> mesures;
    T_Mes mes;
    QList<QByteArray> parties;
    QFile file("config.ini");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
         qDebug("Erreur ouverture du fichier config.ini !");
    } // if fichier pas bon

    int nbLigne=0;
    while (!file.atEnd()) {         // lecture des lignes du fichier
         QByteArray line = file.readLine();
         if (isdigit(line[0])) {    // si le premier car de la ligne est 0-9
             qDebug() << "CONFIG.INI: " << line;
             parties = line.split(';'); // extrait chaque partie de la ligne
             mes.noMes = parties.at(0).toInt();
             strncpy(mes.adrCapteur, parties.at(1).toStdString().c_str(), sizeof(mes.adrCapteur));
             strncpy(mes.nomClasse, parties.at(2).toStdString().c_str(), sizeof(mes.nomClasse));
             strncpy(mes.nomMes, parties.at(3).toStdString().c_str(), sizeof(mes.nomMes));
             strncpy(mes.symbUnit, parties.at(4).toStdString().c_str(), sizeof(mes.symbUnit));
             strncpy(mes.textUnit, parties.at(5).toStdString().c_str(), sizeof(mes.textUnit));
             mesures.append(mes);       // ajout dans la QList
             nbLigne++;
         } // if
    } // while

    mMsg = new CMsg(this);  // instanciation de la file de messages
    int res = mMsg->initialiser(NOMFIC, (int)LETTRE);
    if (res==-1) {
        qDebug("Erreur init file de message !");
    } // if res

    ui->setupUi(this);

    qDebug() << "Main:" << nbLigne << "capteurs vus";
    // création du segment de mémoire partagé contenant la description des capteurs et les valeurs instantanées
    mShm = new QSharedMemory(KEY, this);
    mShm->attach();   // tentative de s'attacher
    if (!mShm->isAttached()) {   // si existe pas alors création
        res = mShm->create(nbLigne*sizeof(T_Mes));
        if (res == false)
            qDebug(mShm->errorString().toStdString().c_str());
    } // if isattached
    // sauvegarde en mémoire partagée et instanciation des objets threads de lecture des mesures
    T_Mes *data = (T_Mes *)mShm->data();

    mNbMesure=0;
    for(int i=0 ; i<nbLigne ; i++) {
        bool inconnu=true;                          // le capteur par défaut n'est pas reconnu
        memcpy(data, &mesures.at(i), sizeof(T_Mes));
        if (!strncmp(mesures.at(i).nomClasse, "CCapteurI2cLm76_Temp", sizeof("CCapteurI2cLm76_Temp"))) {
            qDebug("Capteur LM76 Temp reconnu !");
            mCapteurs.append(new CCapteurI2cLm76_Temp(this, mesures.at(i).noMes,  0x48));  // le thread est créé mais n'est pas lancé
            connect(this, SIGNAL(arretThreadsCapteur()), (CCapteurI2cLm76_Temp *)mCapteurs.at(i), SLOT(stop()));
            mCapteurs.at(i)->start();                   // lancement du thread
            inconnu = false;                           // la mesure est connue
            mNbMesure++;
        } // if LM76
        if (!strncmp(mesures.at(i).nomClasse, "CCapteurI2cHmc5883_Comp", sizeof("CCapteurI2cHmc5883_Comp"))) {
            qDebug("Capteur Compas HMC5883 reconnu !");
            mCapteurs.append(new CCapteurI2cHmc5883_Comp(this, mesures.at(i).noMes, 0x1E));  // le thread est créé mais n'est pas lancé
            connect(this, SIGNAL(arretThreadsCapteur()), (CCapteurI2cHmc5883_Comp *)mCapteurs.at(i), SLOT(stop()));
            mCapteurs.at(i)->start();                   // lancement du thread
            inconnu = false;                           // la mesure est connue
            mNbMesure++;
        } // if 5883
        if (!strncmp(mesures.at(i).nomClasse, "CCapteurI2cHtu21d_HumTemp", sizeof("CCapteurI2cHtu21d_HumTemp"))) {
            qDebug("Capteur Humidité et température HTU21D reconnu !");
            mCapteurs.append(new CCapteurI2cHtu21d_HumTemp(this, mesures.at(i).noMes, 0x40));  // le thread est créé mais n'est pas lancé
            connect(this, SIGNAL(arretThreadsCapteur()), (CCapteurI2cHtu21d_HumTemp *)mCapteurs.at(i), SLOT(stop()));
            mCapteurs.at(i)->start();                   // lancement du thread
            inconnu = false;                           // la mesure est connue
            mNbMesure++;
        } // if 5883
        if (!strncmp(mesures.at(i).nomClasse, "CCapteurSerialGps", sizeof("CCapteurSerialGps"))) {
            qDebug("Capteur GPS reconnu !");
            mCapteurs.append(new CCapteurSerialGps(this, "/dev/ttyAMA0", mesures.at(i).noMes, 9600, 'N', 8));  // le thread est créé mais n'est pas lancé
            connect(this, SIGNAL(arretThreadsCapteur()), (CCapteurSerialGps *)mCapteurs.at(i), SLOT(stop()));
            mCapteurs.at(i)->start();                   // lancement du thread
            inconnu = false;                           // la mesure est connue
            mNbMesure++;
        } // if GPS
        // TO DO Here : autre définition de capteur

        if (inconnu==true)
            qDebug() << "MainWindow: Classe du capteur inconnu !";
        data++; // on passe à l'espace mémoire suivant
    } // for

    if (mNbMesure > 0) {
       // lancement du timer de mise à jour des mesures dans l'IHM
       // optionnel !!! seulement pour test
       mTimer = new QTimer(this);
       mTimer->setInterval(500);
       connect(mTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
       mTimer->start();

       // creation de l'objet incrustation
       mCInc = new CIncruster(this, mMsg, 1000); // 1000ms d'actualisation de l'incrustation
       connect(mMsg, SIGNAL(mailReady(long)), mCInc, SLOT(onMessReady(long)));
    } // if nbMesure

    // création des objets et connexions à la file des messages
    connect(mMsg, SIGNAL(mailReady(long)), this, SLOT(onMessReady(long)));
    mCCam = new CControlerCamera(this, mMsg);
    connect(mMsg, SIGNAL(mailReady(long)), mCCam, SLOT(onMessReady(long)));

    mCCom = new CCommuniquer(this, mMsg);
    connect(mMsg, SIGNAL(mailReady(long)), mCCom, SLOT(onMessReady(long)));
    connect(mCCom, SIGNAL(afficherTexte(QString)), this, SLOT(onAfficherTexte(QString)));
} // constructeur

MainWindow::~MainWindow()
{
    emit arretThreadsCapteur();
    CI2c::freeInstance(); // libère la mémoire du singleton
    mTimer->stop();
    for(int i=0 ; mCapteurs.size() ; i++) {
        mCapteurs.at(i)->quit();
        mCapteurs.at(i)->wait(2000);  // pour attendre que le thread se termine
        delete mCapteurs.at(i);
    } // for
    delete mTimer;
    delete mCCom;
    delete mCCam;
    delete mCInc;
    mShm->detach();
    delete mShm;
    mMsg->detruire();
    delete mMsg;
    delete ui;
} // destructeur

void MainWindow::onMessReady(long type)
{
    qDebug() << "MainWindow: Attention, message reçu de type " << type << " est dans la file de message.";

} // onMessReady

void MainWindow::on_pbLireMessage_clicked()
{
    // envoyer depuis la RPI la commande de la camera
    T_MessOrdre ordre;
    strcpy(ordre.ordre,"GET /");
    strcat(ordre.ordre, ui->cbRep->currentText().toStdString().c_str());
    strcat(ordre.ordre, "/");
    int pos = ui->cbApp->currentText().indexOf(' ');
    strcat(ordre.ordre, ui->cbApp->currentText().left(pos).toStdString().c_str());
    strcat(ordre.ordre,"?t=goprohero&p=%");
    strcat(ordre.ordre, ui->cbVal->currentText().toStdString().c_str());
    strcat(ordre.ordre, " HTTP/1.1\r\n\r\n"); // fin d'entête requête HTTP
    mMsg->sendMessage(TYPE_MESS_ORDRE_CAMERA, &ordre, sizeof(ordre));
} // onLireMessage

void MainWindow::onTimer()
{
    T_Mes *data = (T_Mes *)mShm->constData();
    mShm->lock();
    for(int i=0 ; i<mNbMesure ; i++) {
        switch(data[i].noMes) {
        case 0: ui->lCapteur1->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        case 1: ui->lCapteur2->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        case 2: ui->lCapteur3->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        case 3: ui->lCapteur4->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        case 4: ui->lCapteur5->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        case 5: ui->lCapteur6->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        case 6: ui->lCapteur7->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        case 7: ui->lCapteur8->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        case 8: ui->lCapteur9->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        case 9: ui->lCapteur10->setText(QString(data[i].textUnit)+QString(data[i].valMes)+QString(data[i].symbUnit)); break;
        default:
            ui->teTexte->append("ATTENTION, dépassement de mesure...");
            break;
        } // sw
    } // for
    mShm->unlock();
}

void MainWindow::onAfficherTexte(QString aff)
{
    ui->teTexte->append(aff);
} // onTimer
