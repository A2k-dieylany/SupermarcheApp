#include "client.h"

Client::Client(std::string n) : nom(n) {}

void Client::ajouterProduit(const Produit& p) {
    panier.push_back(p);
}

std::string Client::getNom() const {
    return nom;
}

// Le nombre d'articles est déduit dynamiquement !
int Client::getNbArticles() const {
    return panier.size();
}

// L'intelligence métier : on calcule la facture
double Client::getTotalFacture() const {
    double total = 0.0;
    for (const auto& p : panier) {
        total += p.getPrix();
    }
    return total;
}