#include "supermarche.h"
#include <stdexcept>
#include <iostream>

Supermarche::Supermarche() : compteurId(1), totalServis(0) {}

void Supermarche::initialiser(int nbCaisses) {
    caisses.clear();
    
    // Règle du cahier des charges : on crée toujours 1 caisse express fixe [cite: 217]
    Caisse express(1, true);
    express.ouvrir(); // La caisse express est toujours ouverte 
    caisses.push_back(express);

    // On crée les autres caisses ordinaires
    for (int i = 0; i < nbCaisses; ++i) {
        caisses.push_back(Caisse(i + 2, false)); 
    }
}

void Supermarche::ouvrirCaisse(int numero) {
    for (auto& c : caisses) {
        if (c.getNumero() == numero) {
            c.ouvrir();
            return;
        }
    }
    throw std::runtime_error("Erreur : La caisse " + std::to_string(numero) + " n'existe pas.");
}

void Supermarche::fermerCaisse(int numero) {
    for (auto& c : caisses) {
        if (c.getNumero() == numero) {
            c.fermer();
            return;
        }
    }
    throw std::runtime_error("Erreur : La caisse " + std::to_string(numero) + " n'existe pas.");
}

Caisse& Supermarche::choisirCaisse(int nbArticles) {
    // 1. Règle Express : 10 articles ou moins 
    if (nbArticles <= 10) {
        for (auto& c : caisses) {
            if (c.isExpress() && c.estOuverte()) return c;
        }
    }

    // 2. Règle Ordinaire : Trouver la caisse la moins chargée 
    Caisse* meilleureCaisse = nullptr;
    int minClients = -1;

    for (auto& c : caisses) {
        if (!c.isExpress() && c.estOuverte()) {
            if (minClients == -1 || c.getTailleFile() < minClients) {
                minClients = c.getTailleFile();
                meilleureCaisse = &c; // On garde un pointeur vers la meilleure caisse
            }
        }
    }

    if (meilleureCaisse == nullptr) {
        throw std::runtime_error("Aucune caisse ordinaire n'est ouverte !");
    }

    return *meilleureCaisse; // On déréférence le pointeur pour renvoyer la référence
}

void Supermarche::ajouterClient(const std::string& nom, int nbArticles) {
    Caisse& caisseCible = choisirCaisse(nbArticles);
    Client nouveauClient(compteurId++, nom, nbArticles);
    
    caisseCible.ajouterClient(nouveauClient);
    std::cout << "Le client " << nom << " a ete envoye a la caisse " << caisseCible.getNumero() << std::endl;
}

void Supermarche::servirClient(int numeroCaisse) {
    for (auto& c : caisses) {
        if (c.getNumero() == numeroCaisse) {
            Client servi = c.servirClient();
            totalServis++;
            std::cout << "Caisse " << numeroCaisse << " a servi : " << servi.getNom() << std::endl;
            return;
        }
    }
    throw std::runtime_error("Erreur : La caisse " + std::to_string(numeroCaisse) + " n'existe pas.");
}

std::vector<Caisse>& Supermarche::getCaisses() { return caisses; }
int Supermarche::getTotalClientsServis() const { return totalServis; }