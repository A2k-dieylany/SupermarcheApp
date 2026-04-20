#pragma once
#include <string>
#include <ctime>

class Client {
public:
    Client(int id, const std::string& nom, int nbArticles);
    
    int getId() const;
    std::string getNom() const;
    int getNbArticles() const;
    time_t getTempsArrivee() const;
    
    std::string toString() const;

private:
    int id;
    std::string nom;
    int nbArticles;
    time_t tempsArrivee;
};