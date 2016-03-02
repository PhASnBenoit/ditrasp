#ifndef GLOBAL_H
#define GLOBAL_H

#include <QSharedMemory>

#define NOMFIC "dit"  // pour générer la clef de la file de messages
#define LETTRE 'd'    // pour générer la clef de la file de messages
#define KEY "dit"     // clef pour le segment de mémoire partagé
#define AFFMAX 50      // long max de la valeur de la mesure
#define NBMAXCAPT 9     // Nb max de capteurs sur le drone (9 affichage au max d'incrustation)

// structure du fichier de configuration config.ini
typedef struct {
    int noMes;              // numéro de la mesure
    char nomClasse[50];     // nom de la classe de gestion du capteur
    char nomMes[50];        // nom de la mesure
    char symbUnit[11];      // Symbole de l'unité de la mesure
    char textUnit[50];      // Texte de l'unité
    char valMes[11];        // Valeur instantannée de la mesure
} T_Mes;

#endif // GLOBAL_H
