#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    msg = new CMsg(this);  // instanciation de la file de messages
    int res = msg->initialiser(NOMFIC, (int)LETTRE);
    if (res==-1) {
        qDebug("Erreur init file de message !");
    } // if res

    // lecture du fichier de configuration config.ini qui identifie les capteurs présents sur le drone
    QList<T_Mes> mesures;
    T_Mes mes;
    QList<QByteArray> parties;
    QFile file("config.ini");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         qDebug("Erreur ouverture du fichier config.ini !");
    int nbLigne=0;
    while (!file.atEnd()) {         // lecture des lignes du fichier
         QByteArray line = file.readLine();
         if (isdigit(line[0])) {    // si le premier car de la ligne est 0-9
             parties = line.split(';'); // extrait chaque partie de la ligne
             mes.noMes = parties.at(0).toInt();
             strncpy(mes.nomClasse, parties.at(1).toStdString().c_str(), sizeof(mes.nomClasse));
             strncpy(mes.nomMes, parties.at(2).toStdString().c_str(), sizeof(mes.nomMes));
             strncpy(mes.symbUnit, parties.at(3).toStdString().c_str(), sizeof(mes.symbUnit));
             strncpy(mes.textUnit, parties.at(4).toStdString().c_str(), sizeof(mes.textUnit));
             mesures.append(mes);       // ajout dans la QList
             nbLigne++;
         } // if
    } // while

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
        if (!strncmp(mesures.at(i).nomClasse, "CCapteurSTH15_Temp", sizeof("CCapteurSTH15_Temp"))) {
            qDebug("Capteur STH15 Temp reconnu !");
            capteurs.append(new CCapteurI2cLm76_Temp(this, mesures.at(i).noMes));  // le thread est créé mais n'est pas lancé
            capteurs.at(i)->start();                   // lancement du thread
            inconnu = false;                           // la mesure est connue
            nbMesure++;
        } // if STH15
        // USAGE ULTERIEUR
/*        if (!strncmp(mesures.at(i).nomClasse, "CCapteurSTH15_Hum", sizeof("CCapteurSTH15_Hum"))) {
            qDebug("Capteur STH15 Hum reconnu !");
            capteurs.append(new CCapteurSTH15_Hum(this, mesures.at(i).noMes));  // le thread est créé mais n'est pas lancé
            capteurs.at(i)->start();                   // lancement du thread
            inconnu = false;                           // la mesure est connue
            nbMesure++;
        } // if STH15
*/        // TO DO Here : autre définition des capteurs
        if (inconnu)
            qDebug("Classe du capteur inconnu !");
        data++; // on passe à l'espace mémoire suivant
    } // for

     // lancement du timer de mise à jour des mesures dans l'IHM
    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

    // création des objets et connexions à la file des messages
    pc_inc = new PC_Incruster(this, msg, 500); // 500 ms d'actualisation de l'incrustation
    connect(msg, SIGNAL(mailReady(long)), pc_inc, SLOT(onMessReady(long)));
    pc_contcam = new PC_ControlerCamera(this, msg);
    connect(msg, SIGNAL(mailReady(long)), pc_contcam, SLOT(onMessReady(long)));
    pc_com = new PC_Communiquer(this, msg);
    connect(msg, SIGNAL(mailReady(long)), pc_com, SLOT(onMessReady(long)));
} // constructeur

MainWindow::~MainWindow()
{
    CI2c::freeInstance();
    timer->stop();
    delete timer;
    delete pc_com;
    delete pc_contcam;
    delete pc_inc;
    for(int i=0 ; i<capteurs.size() ; i++)
        delete capteurs.at(i);
    shm->detach();
    delete shm;
    msg->detruire();
    delete msg;
    delete ui;
    //delete msg;
}

void MainWindow::onMessReady(long type)
{
    QMessageBox::warning(0, tr("Attention : Message envoyé"),
                         tr("Le message de type ")+QString::number(type)+
                         tr(" est dans la file de message."), QMessageBox::Ok);
}

void MainWindow::on_pbLireMessage_clicked()
{
    // essai d'envoi de message
        T_MessMes mess;
        mess.corps.valInt = 1234;
        msg->sendMessage(TYPE_MESS_MESURE,&mess, sizeof(T_MessMes));
}

void MainWindow::onTimer()
{
    T_Mes *data = (T_Mes *)shm->constData();
    QString unite;
    shm->lock();
    for(int i=0 ; i<nbMesure ; i++) {
        if (!strncmp(data[i].nomClasse,"CCapteurSTH15_Temp",sizeof("CCapteurSTH15_Temp"))) {
            //unite=QString("°C");
            ui->lTemp->setText(QString(data[i].valMes));
            ui->lTemp->setText(ui->lTemp->text()+unite);
        } // if
    } // for
    shm->unlock();
}
