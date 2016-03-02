#ifndef PC_CONTROLERCAMERA_H
#define PC_CONTROLERCAMERA_H

#include <QObject>
#include <stdio.h>
#include "cmsg.h"


class PC_ControlerCamera : public QObject
{
    Q_OBJECT

public:
    explicit PC_ControlerCamera(QObject *parent = 0, CMsg *msg = 0);

private:
    CMsg *pMsg;

signals:

private slots:
    void onMessReady(long type);

public slots:

};

#endif // PC_CONTROLERCAMERA_H
