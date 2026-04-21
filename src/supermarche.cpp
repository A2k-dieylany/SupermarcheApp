#include "supermarche.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>

// --- INITIALISATION ---
Supermarche::Supermarche() {
    catalogue.push_back(Produit(1, "Baguette", 0.50, "Boulangerie"));
    catalogue.push_back(Produit(2, "Lait de vache", 1.20, "Frais"));
    catalogue.push_back(Produit(3, "Riz (1kg)", 2.50, "Épicerie"));
    catalogue.push_back(Produit(4, "Café moulu", 3.80, "Épicerie"));
    catalogue.push_back(Produit(5, "Savon", 1.50, "Hygiène"));
}

void Supermarche::initialiser(int nbCaisses) {
    caisses.clear();
    for (int i = 1; i <= nbCaisses; i++) {
        caisses.push_back(Caisse(i, i == 1)); 
    }
}

// --- LOGIQUE MÉTIER ---
void Supermarche::ajouterClient(const Client& client) {
    if (caisses.empty()) {
        throw std::runtime_error("Erreur : Aucune caisse n'est installée.");
    }

    int nbArticles = client.getNbArticles();
    Caisse* caisseChoisie = nullptr;
    
    // 1. Cherche une caisse express vide/la moins pleine
    if (nbArticles <= 10) {
        for (auto& c : caisses) {
            if (c.estOuverte() && c.isExpress()) {
                if (!caisseChoisie || c.getTailleFile() < caisseChoisie->getTailleFile()) {
                    caisseChoisie = &c;
                }
            }
        }
    }

    // 2. Sinon, cherche une caisse normale
    if (!caisseChoisie) {
        for (auto& c : caisses) {
            if (c.estOuverte() && !c.isExpress()) {
                if (!caisseChoisie || c.getTailleFile() < caisseChoisie->getTailleFile()) {
                    caisseChoisie = &c;
                }
            }
        }
    }

    // 3. En dernier recours, n'importe quelle caisse ouverte
    if (!caisseChoisie) {
        for (auto& c : caisses) {
            if (c.estOuverte()) {
                if (!caisseChoisie || c.getTailleFile() < caisseChoisie->getTailleFile()) {
                    caisseChoisie = &c;
                }
            }
        }
    }

    if (caisseChoisie) {
        caisseChoisie->ajouterClient(client);
    } else {
        throw std::runtime_error("Toutes les caisses sont fermées.");
    }
}

void Supermarche::servirClient(int numeroCaisse) {
    for (auto& c : caisses) {
        if (c.getNumero() == numeroCaisse) {
            c.servirClient();
            totalClientsServis++;
            return;
        }
    }
    throw std::runtime_error("Erreur : La caisse n'existe pas.");
}

void Supermarche::ouvrirCaisse(int numero) {
    for (auto& c : caisses) {
        if (c.getNumero() == numero) {
            c.ouvrir();
            return;
        }
    }
    throw std::runtime_error("Erreur : La caisse n'existe pas.");
}

void Supermarche::fermerCaisse(int numero) {
    for (auto& c : caisses) {
        if (c.getNumero() == numero) {
            c.fermer();
            return;
        }
    }
    throw std::runtime_error("Erreur : La caisse n'existe pas.");
}

void Supermarche::viderCaisse(int numero) {
    for (auto& c : caisses) {
        if (c.getNumero() == numero) {
            c.vider();
            return;
        }
    }
    throw std::runtime_error("Erreur : La caisse n'existe pas.");
}

// --- GETTERS (C'est eux qui manquaient !) ---
std::vector<Caisse>& Supermarche::getCaisses() {
    return caisses;
}

int Supermarche::getTotalClientsServis() const {
    return totalClientsServis;
}

const std::vector<Produit>& Supermarche::getCatalogue() const {
    return catalogue;
}

// --- GESTION DE L'INVENTAIRE (ADMIN) ---
void Supermarche::ajouterProduit(const Produit& p) {
    // Vérifie si l'ID existe déjà pour éviter les doublons
    for (const auto& prod : catalogue) {
        if (prod.getId() == p.getId()) {
            throw std::runtime_error("Erreur : Un produit avec cet ID existe déjà.");
        }
    }
    catalogue.push_back(p);
}

void Supermarche::supprimerProduit(int id) {
    // std::remove_if est une fonction C++ très puissante pour filtrer un tableau
    auto it = std::remove_if(catalogue.begin(), catalogue.end(),
        [id](const Produit& p) { return p.getId() == id; });
        
    if (it == catalogue.end()) {
        throw std::runtime_error("Erreur : Produit introuvable.");
    }
    
    catalogue.erase(it, catalogue.end());
}