#include "client.h"

Client::Client(std::string n) : nom(n) {}

void Client::ajouterProduit(const Produit& p) {
    panier.push_back(p);
}

std::string Client::getNom() const {
    return nom;
}

int Client::getNbArticles() const {
    return panier.size();
}

// --- NOUVEAU : LE CALCUL DE LA FACTURE ---
double Client::getTotalFacture() const {
    double total = 0.0;
    for (const auto& p : panier) {
        total += p.getPrix(); // Ajoute le prix de chaque produit
    }
    return total;
}