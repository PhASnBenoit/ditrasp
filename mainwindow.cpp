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

    msg = new CMsg(this);  // instanciation de la file de messages
    int res = msg->initialiser(NOMFIC, (int)LETTRE);
    if (res==-1) {
        qDebug("Erreur init file de message !");
    } // if res

    ui->setupUi(this);

    qDebug() << "Main:" << nbLigne << "capteurs vus";
    // création du segment de mémoire partagé contenant la description des capteurs et les valeurs instantanées
    shm = new QSharedMemory(KEY, this);
    shm->attach();   // tentative de s'attacher
    if (!shm->isAttached()) {   // si existe pas alors création
        res = shm->create(nbLigne*sizeof(T_Mes));
        if (res == false)
            qDebug(shm->errorString().toStdString().c_str());
    } // if isattached
    // sauvegarde en mémoire partagée et instanciation des objets threads de lecture des mesures
    T_Mes *data = (T_Mes *)shm->data();

    nbMesure=0;
    for(int i=0 ; i<nbLigne ; i++) {
        bool inconnu=true;                          // le capteur par défaut n'est pas reconnu
        memcpy(data, &mesures.at(i), sizeof(T_Mes));
        if (!strncmp(mesures.at(i).nomClasse, "CCapteurI2cLm76_Temp", sizeof("CCapteurI2cLm76_Temp"))) {
            qDebug("Capteur LM76 Temp reconnu !");
            capteurs.append(new CCapteurI2cLm76_Temp(this, mesures.at(i).noMes,  0x48));  // le thread est créé mais n'est pas lancé
            connect(this, SIGNAL(arretThreadsCapteur()), (CCapteurI2cLm76_Temp *)capteurs.at(i), SLOT(stop()));
            capteurs.at(i)->start();                   // lancement du thread
            inconnu = false;                           // la mesure est connue
            nbMesure++;
        } // if LM76
        if (!strncmp(mesures.at(i).nomClasse, "CCapteurI2cHmc5883_Comp", sizeof("CCapteurI2cHmc5883_Comp"))) {
            qDebug("Capteur Compas HMC5883 reconnu !");
            capteurs.append(new CCapteurI2cHmc5883_Comp(this, mesures.at(i).noMes, 0x1E));  // le thread est créé mais n'est pas lancé
            connect(this, SIGNAL(arretThreadsCapteur()), (CCapteurI2cHmc5883_Comp *)capteurs.at(i), SLOT(stop()));
            capteurs.at(i)->start();                   // lancement du thread
            inconnu = false;                           // la mesure est connue
            nbMesure++;
        } // if 5883
        if (!strncmp(mesures.at(i).nomClasse, "CCapteurSerialGps", sizeof("CCapteurSerialGps"))) {
            qDebug("Capteur GPS reconnu !");
            capteurs.append(new CCapteurSerialGps(this, "/dev/ttyAMA0", mesures.at(i).noMes, 9600, 'N', 8));  // le thread est créé mais n'est pas lancé
            connect(this, SIGNAL(arretThreadsCapteur()), (CCapteurSerialGps *)capteurs.at(i), SLOT(stop()));
            capteurs.at(i)->start();                   // lancement du thread
            inconnu = false;                           // la mesure est connue
            nbMesure++;
        } // if GPS
        // TO DO Here : autre définition de capteur

        if (inconnu==true)
            qDebug("Classe du capteur inconnu !");
        data++; // on passe à l'espace mémoire suivant
    } // for

    if (nbMesure > 0) {
       // lancement du timer de mise à jour des mesures dans l'IHM
       timer = new QTimer(this);
       timer->setInterval(500);
       connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
       timer->start();
       pc_inc = new CIncruster(this, msg, 1000); // 1000ms d'actualisation de l'incrustation
       connect(msg, SIGNAL(mailReady(long)), pc_inc, SLOT(onMessReady(long)));
    } // if nbMesure
    // création des objets et connexions à la file des messages
    connect(msg, SIGNAL(mailReady(long)), this, SLOT(onMessReady(long)));
    pc_contcam = new CControlerCamera(this, msg);
    connect(msg, SIGNAL(mailReady(long)), pc_contcam, SLOT(onMessReady(long)));
    pc_com = new CCommuniquer(this, msg);
    connect(msg, SIGNAL(mailReady(long)), pc_com, SLOT(onMessReady(long)));
} // constructeur

MainWindow::~MainWindow()
{
    emit arretThreadsCapteur();
    CI2c::freeInstance(); // libère la mémoire du singleton
    timer->stop();
    for(int i=0 ; i<capteurs.size() ; i++) {
        delete capteurs.at(i);
    } // for
    delete timer;
    delete pc_com;
    delete pc_contcam;
    delete pc_inc;
    shm->detach();
    delete shm;
    msg->detruire();
    delete msg;
    delete ui;
} // destructeur

void MainWindow::onMessReady(long type)
{
    qDebug() << "MainWindow: Attention, message reçu de type " << type << " est dans la file de message.";

} // onMessReady

void MainWindow::on_pbLireMessage_clicked()
{
    T_MessOrdre ordre;
    strcpy(ordre.ordre,"GET /");
    strcat(ordre.ordre, ui->cbRep->currentText().toStdString().c_str());
    strcat(ordre.ordre, "/");
    int pos = ui->cbApp->currentText().indexOf(' ');
    strcat(ordre.ordre, ui->cbApp->currentText().left(pos).toStdString().c_str());
    strcat(ordre.ordre,"?t=Tototom384&p=%");
    strcat(ordre.ordre, ui->cbVal->currentText().toStdString().c_str());
    strcat(ordre.ordre, " HTTP/1.1\r\n\r\n"); // fin d'entête requête HTTP
    msg->sendMessage(TYPE_MESS_ORDRE_CAMERA, &ordre, sizeof(ordre));
} // onLireMessage

void MainWindow::onTimer()
{
    T_Mes *data = (T_Mes *)shm->constData();
    shm->lock();
    for(int i=0 ; i<nbMesure ; i++) {
        switch(i) {
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
    shm->unlock();
} // onTimer


/*
void MainWindow::on_pbQuitter_clicked()
{
    this->destroy();
}
*/
