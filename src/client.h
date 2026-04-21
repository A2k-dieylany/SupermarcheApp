#pragma once
#include <string>
#include <vector>
#include "produit.h"

class Client {
private:
    std::string nom;
    std::vector<Produit> panier; // <-- Le fameux panier !

public:
    Client(std::string n);
    
    void ajouterProduit(const Produit& p);
    
    std::string getNom() const;
    int getNbArticles() const;
    double getTotalFacture() const; // Nouvelle fonctionnalité !
};