#pragma once
#include <queue>
#include "client.h"

class Caisse {
private:
    int numero;
    bool ouverte;
    bool express;
    std::queue<Client> file;

public:
    Caisse(int num, bool expr = false);
    
    int getNumero() const;
    bool estOuverte() const;
    bool isExpress() const;
    int getTailleFile() const;
    int getTempsAttente() const;
    
    void ouvrir();
    std::queue<Client> fermer();
    void vider();
    
    void ajouterClient(const Client& client);
    
    // --- NOUVEAU : ENCAISSEMENT ---
    // La fonction retourne maintenant un 'double' (l'argent payé) au lieu de 'void'
    double servirClient(); 
};