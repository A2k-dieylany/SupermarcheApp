#include "client.h"

Client::Client(int id, const std::string& nom, int nbArticles) 
    : id(id), nom(nom), nbArticles(nbArticles), tempsArrivee(time(nullptr)) {
}

int Client::getId() const { return id; }
std::string Client::getNom() const { return nom; }
int Client::getNbArticles() const { return nbArticles; }
time_t Client::getTempsArrivee() const { return tempsArrivee; }

std::string Client::toString() const {
    return "Client #" + std::to_string(id) + " : " + nom + 
           " (" + std::to_string(nbArticles) + " articles)";
}