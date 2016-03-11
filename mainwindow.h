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
    CMsg *msg;  // file des messages
    QSharedMemory *shm;  // segment de mémoire partagé

    CCommuniquer *pc_com;
    CIncruster *pc_inc;
    CControlerCamera *pc_contcam;

    QList<QThread *> capteurs;   // tableau de thread des capteurs
    QTimer *timer;
    int nbMesure;

private slots:
    void onMessReady(long type);
    void on_pbLireMessage_clicked();
    void onTimer();
    //void on_pbQuitter_clicked();
};

#endif // MAINWINDOW_H
