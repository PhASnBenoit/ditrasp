#include "cgpio.h"

CGpio::CGpio(int addr)
{
    mAddr = addr;
    init();
} // constructeur

int CGpio::init()
{
    int fd;
    char buffer[3];

    sprintf(filename,"/sys/class/gpio/%s",EXPORT);
    fd = open(filename, O_WRONLY);
    if (fd < 0) {
        qDebug() << "CGpio::init: Erreur d'ouverture du fichier " << filename;
    } // if erreur open

    sprintf(buffer,"%d", mAddr);
    if( write(fd, buffer, strlen(buffer)) != strlen(buffer))
    {
        qDebug() << "CGpio::init: Erreur ou existe deja" << endl;
    } else
        qDebug() << "CGpio::CGpio : Export de " << mAddr << " reussi " << endl;
    close(fd);
    usleep(50000);

    sprintf(filename,"/sys/class/gpio/gpio%d/",mAddr);

    QString command = "sudo chmod -R 777 "+QString(filename);

    QProcess *proc= new QProcess(this);
    qDebug() << "CGpio:init : " << command << endl;
    proc->start(command);
    sleep(2);

    return 1;
} // init
	
CGpio::~CGpio()
{

} // destructeur


int CGpio::lire()
{
	int fd;
	char value_str[1];
	char gpioPath[50];	
	
	sprintf(gpioPath,filename);
	strcat(gpioPath,DIRECTION);
	char dir[2];
	sprintf(dir,IN);
	fd = open(gpioPath, O_WRONLY);
	write(fd, dir,sizeof(dir));
	close(fd);
	
	sprintf(gpioPath,filename);
	strcat(gpioPath,VALUE);;
	fd = open(gpioPath, O_RDONLY);
    if (read(fd, value_str, 1)==-1) {
        qDebug() << "Cgpio::lire : Erreur de lecture";
		return(-1);
	} 
	close(fd);
 
	return(atoi(value_str));
}
	
int CGpio::ecrire(int value)
{
	int fd;
	char gpioPath[50];
	sprintf(gpioPath,filename);
	strcat(gpioPath,DIRECTION);
	//cout << gpioPath << endl;
	char dir[3];
	sprintf(dir,OUT);
	//cout << dir << endl;
	
	fd = open(gpioPath, O_WRONLY);
	write(fd, dir,sizeof(dir));
	close(fd);
	
	sprintf(gpioPath,filename);
	strcat(gpioPath,VALUE);
	//cout << gpioPath << endl;
	
	fd = open(gpioPath, O_WRONLY);
	
	char value_str[1];
	if(value==0) value_str[0]='0';
	else value_str[0]='1';

	if (write(fd, value_str,1)!=1){
        qDebug() << "Cgpio::ecrire : Erreur d'ecriture sur " << gpioPath << " : " << value_str;
		return(-1);
	}
	close(fd);
	return(0);
}	
