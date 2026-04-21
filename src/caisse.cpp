#include "caisse.h"
#include <stdexcept>
#include <iostream>

Caisse::Caisse(int num, bool expr) : numero(num), ouverte(false), express(expr) {}

int Caisse::getNumero() const {
    return numero;
}

bool Caisse::estOuverte() const {
    return ouverte;
}

bool Caisse::isExpress() const {
    return express;
}

int Caisse::getTailleFile() const {
    return file.size();
}

int Caisse::getTempsAttente() const {
    // Estimation : 120 secondes par client en attente
    return file.size() * 120;
}

void Caisse::ouvrir() {
    ouverte = true;
}

void Caisse::fermer() {
    if (!file.empty()) {
        throw std::runtime_error("Impossible de fermer : il y a encore des clients !");
    }
    ouverte = false;
}

// La fameuse méthode d'urgence
void Caisse::vider() {
    std::queue<Client> fileVide;
    std::swap(file, fileVide);
}

void Caisse::ajouterClient(const Client& client) {
    if (!ouverte) {
        throw std::runtime_error("La caisse est fermee.");
    }
    // On vérifie le panier dynamique du client !
    if (express && client.getNbArticles() > 10) {
        throw std::runtime_error("Trop d'articles pour la caisse express.");
    }
    file.push(client);
}

void Caisse::servirClient() {
    if (file.empty()) {
        throw std::runtime_error("Aucun client a servir.");
    }
    // Retire le premier client de la file
    file.pop();
}