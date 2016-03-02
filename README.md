# ditrasp
Développement du logiciel embarqué sur un drone équipé d'une Raspberry pi 2 B+
L'objectif est de piloter les équipements suivants d'un drone :  
- capteurs GPS, humidité, température, altimètre, etc,
- caméra GoPro,
- incrustateur de textes sur la vidéo de retour,
- moyen de communication radio avec la station au sol,
- moyen de communication WIFI avec la GoPro,
depuis une station au sol.

Le matériel choisi est une carte Raspberry PI2 B+, une caméra GoPro Hero 3 Black.
Les capteurs et incrustateur peuvent changer.
Le développement de la station au sol fera l'objet d'un autre repository.

Le fond de l'application est développé en cross développement avec QT 5.6.0 pour bénéficier des signaux/slots entre les objets.

L'applicatif comprend du multithreading pour les capteurs, des classes singleton, 
une file de messages, un segment de mémoire partagé protégé par sémaphore (QShareMemory).
La version 1.0 du logiciel sera livrée d'ici fin juin 2016.
