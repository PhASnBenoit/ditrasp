#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QTimer>
#include "global.h"
#include "cmsg.h"
#include "ci2c.h"
#include "ccapteuri2clm76_temp.h"
#include "ccapteuri2chtu21d_humtemp.h"
#include "ccapteuri2chmc5883_comp.h"
#include "ccapteurserialgps.h"
#include "ccommuniquer.h"
#include "ccontrolercamera.h"
#include "cincruster.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CMsg *mMsg;  // file des messages
    QSharedMemory *mShm;  // segment de mémoire partagé

    CCommuniquer *mCCom;
    CIncruster *mCInc;
    CControlerCamera *mCCam;

    QList<QThread *> mCapteurs;   // tableau de thread des capteurs
    QTimer *mTimer;
    int mNbMesure;

private slots:
    void onMessReady(long type);
    void on_pbLireMessage_clicked();
    void onTimer();
    void onAfficherTexte(QString aff);
    void onLancerThreads();

signals:
    void arretThreadsCapteur();
};

#endif // MAINWINDOW_H
