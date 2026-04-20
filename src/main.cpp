#include <iostream>
#include "supermarche.h"
#include "serveur.h"

int main() {
    Supermarche sm;
    
    // On initialise le supermarché (1 express + 2 ordinaires)
    sm.initialiser(3); 
    
    // On ouvre les caisses
    sm.ouvrirCaisse(2);
    sm.ouvrirCaisse(3);
    
    // On démarre le serveur web
    Serveur serveur;
    serveur.demarrer(sm);
    
    return 0;
}