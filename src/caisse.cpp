#include "caisse.h"
#include <stdexcept>
#include <iostream>

Caisse::Caisse(int num, bool expr) : numero(num), ouverte(false), express(expr) {}

int Caisse::getNumero() const { return numero; }
bool Caisse::estOuverte() const { return ouverte; }
bool Caisse::isExpress() const { return express; }
int Caisse::getTailleFile() const { return file.size(); }

int Caisse::getTempsAttente() const {
    // Estimation : 120 secondes par client en attente
    return file.size() * 120;
}

void Caisse::ouvrir() { ouverte = true; }

std::queue<Client> Caisse::fermer() {
    ouverte = false; // On ferme la caisse
    
    std::queue<Client> clientsOrphelins;
    
    // std::swap est une astuce C++ ultra-rapide pour échanger deux variables
    // Ici, on vide "file" et on met tous les clients dans "clientsOrphelins"
    std::swap(file, clientsOrphelins); 
    
    return clientsOrphelins; // On renvoie les clients au supermarché
}
void Caisse::vider() {
    std::queue<Client> fileVide;
    std::swap(file, fileVide);
}

void Caisse::ajouterClient(const Client& client) {
    if (!ouverte) {
        throw std::runtime_error("La caisse est fermee.");
    }
    if (express && client.getNbArticles() > 10) {
        throw std::runtime_error("Trop d'articles pour la caisse express.");
    }
    file.push(client);
}

// --- NOUVEAU : LE SCANNER DE CAISSE ---
double Caisse::servirClient() {
    if (file.empty()) {
        throw std::runtime_error("Aucun client a servir.");
    }
    
    // 1. On regarde le client qui est premier dans la file
    Client clientServi = file.front();
    
    // 2. On calcule le total de ses achats
    double montantPaye = clientServi.getTotalFacture();
    
    // 3. Le client a payé, il quitte la file
    file.pop();
    
    // 4. La caisse transmet l'argent au coffre du Supermarché
    return montantPaye;
}