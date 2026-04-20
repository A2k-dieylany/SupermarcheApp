#include "caisse.h"

// Le constructeur
Caisse::Caisse(int numero, bool estExpress) 
    : numero(numero), ouvert(false), express(estExpress) {
}

void Caisse::ajouterClient(const Client& c) {
    if (!ouvert) {
        throw std::runtime_error("Impossible d'ajouter : la caisse " + std::to_string(numero) + " est fermee.");
    }
    file.push(c); // push() ajoute à la fin de la file
}

Client Caisse::servirClient() {
    if (file.empty()) {
        throw std::runtime_error("Impossible de servir : la caisse " + std::to_string(numero) + " est vide.");
    }
    
    Client c = file.front(); // front() lit le premier élément
    file.pop();              // pop() le retire définitivement de la file
    return c;
}

bool Caisse::estVide() const { return file.empty(); }

int Caisse::getTailleFile() const { return file.size(); }

int Caisse::getTempsAttente() const {
    int tempsTotal = 0;
    // Une std::queue ne permet pas de faire une boucle "for" classique.
    // On fait donc une copie temporaire pour calculer le temps total.
    std::queue<Client> copieFile = file;
    while (!copieFile.empty()) {
        tempsTotal += copieFile.front().getNbArticles() * 5; // 5 secondes par article
        copieFile.pop();
    }
    return tempsTotal;
}

bool Caisse::estOuverte() const { return ouvert; }
bool Caisse::isExpress() const { return express; }
int Caisse::getNumero() const { return numero; }

void Caisse::ouvrir() { ouvert = true; }

void Caisse::fermer() {
    if (!file.empty()) {
        throw std::runtime_error("Impossible de fermer la caisse " + std::to_string(numero) + " : il reste des clients !");
    }
    ouvert = false;
}