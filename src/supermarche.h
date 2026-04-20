#pragma once
#include <vector>
#include <string>
#include "caisse.h"

class Supermarche {
public:
    Supermarche();
    
    void initialiser(int nbCaisses); // Crée les caisses (dont 1 express)
    void ouvrirCaisse(int numero);
    void fermerCaisse(int numero);
    
    void ajouterClient(const std::string& nom, int nbArticles);
    void servirClient(int numeroCaisse);
    
    std::vector<Caisse>& getCaisses();
    int getTotalClientsServis() const;

private:
    std::vector<Caisse> caisses;
    int compteurId;  // Pour donner un ID unique à chaque client
    int totalServis; // Statistiques globales

    // Méthode privée essentielle : l'algorithme d'orientation
    Caisse& choisirCaisse(int nbArticles); 
};