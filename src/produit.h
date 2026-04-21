#pragma once
#include <string>

class Produit {
private:
    int id;
    std::string nom;
    double prix;
    std::string categorie;

public:
    // Constructeur par défaut (nécessaire pour C++)
    Produit() : id(0), prix(0.0) {} 
    
    // Constructeur principal
    Produit(int i, std::string n, double p, std::string cat) 
        : id(i), nom(n), prix(p), categorie(cat) {}

    // Les "Getters" pour lire les informations
    int getId() const { return id; }
    std::string getNom() const { return nom; }
    double getPrix() const { return prix; }
    std::string getCategorie() const { return categorie; }
};