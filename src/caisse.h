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
    void fermer();
    void vider(); // <-- Elle est bien là !
    
    void ajouterClient(const Client& client);
    void servirClient();
};