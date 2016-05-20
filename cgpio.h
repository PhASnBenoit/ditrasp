#ifndef CGPIO_H
#define CGPIO_H

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>				
#include <iostream>
#include <time.h>
#include <sys/select.h> 
#include <string.h>
#include <cstdlib>

#define EXPORT 		"export"
#define DIRECTION 	"direction"
#define VALUE 		"value"
#define IN          "in"
#define OUT 		"out"

class CGpio : public QObject
{
    Q_OBJECT

public :
    explicit CGpio(int addr);
    ~CGpio();
    int lire();
    int ecrire(int value);

private :
    char filename[50];
    int init();

    int mAddr;
};

#endif // CGPIO_H
