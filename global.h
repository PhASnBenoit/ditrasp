#ifndef GLOBAL_H
#define GLOBAL_H

#include <QSharedMemory>

#define NOMFIC "./ditRasp"  // pour générer la clef de la file de messages
#define LETTRE 'd'    // pour générer la clef de la file de messages
#define KEY "./ditRasp"     // clef pour le segment de mémoire partagé
#define AFFMAX 50      // long max de la valeur de la mesure
#define NBMAXCAPT 9     // Nb max de capteurs sur le drone (9 affichage au max d'incrustation)

typedef unsigned char UC;
typedef unsigned short US;

// structure du fichier de configuration config.ini
typedef struct {
    int noMes;              // numéro de la mesure
    char adrCapteur[AFFMAX];    // Adresse ou fichier d'accès au capteur
    char nomClasse[AFFMAX];     // nom de la classe de gestion du capteur
    char nomMes[AFFMAX];        // nom de la mesure
    char symbUnit[AFFMAX];      // Symbole de l'unité de la mesure
    char textUnit[AFFMAX];      // Texte de l'unité
    char valMes[AFFMAX];        // Valeur instantannée de la mesure
} T_Mes;

#endif // GLOBAL_H
