#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QList>
#include <QTimer>
#include "global.h"
#include "cmsg.h"
#include "ci2c.h"
#include "ccapteuri2clm76_temp.h"
#include "pc_communiquer.h"
#include "pc_controlercamera.h"
#include "pc_incruster.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    CMsg *msg;  // file des messages
    QSharedMemory *shm;  // segment de mémoire partagé

private:
    Ui::MainWindow *ui;
    PC_Communiquer *pc_com;
    PC_Incruster *pc_inc;
    PC_ControlerCamera *pc_contcam;
    QList<QThread *> capteurs;   // tableau de thread des capteurs
    QTimer *timer;
    int nbMesure;

private slots:
    void onMessReady(long type);
    void on_pbLireMessage_clicked();
    void onTimer();
};

#endif // MAINWINDOW_H
