#pragma once
#include <queue>
#include <stdexcept>
#include "client.h"


class Caisse {
public:
    Caisse(int numero, bool estExpress = false);
    
    void ajouterClient(const Client& c);
    Client servirClient(); 
    
    bool estVide() const;
    int getTailleFile() const;
    int getTempsAttente() const; // Estimation : 5 secondes par article
    
    bool estOuverte() const;
    bool isExpress() const;
    int getNumero() const;
    
    void ouvrir();
    void fermer(); // Doit bloquer si la file n'est pas vide
    

private:
    int numero;
    bool ouvert;
    bool express;
    std::queue<Client> file; // Notre fameuse file d'attente
};