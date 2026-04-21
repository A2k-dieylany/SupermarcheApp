#pragma once
#include <vector>
#include <sqlite3.h> // Moteur de base de données professionnel
#include "caisse.h"
#include "produit.h"

class Supermarche {
private:
    std::vector<Caisse> caisses;
    int totalClientsServis = 0;
    
    // --- NOUVEAU : LE COFFRE-FORT DU GÉRANT ---
    double chiffreAffairesTotal = 0.0; 
    
    // La connexion à la base de données SQLite
    sqlite3* db; 

    // Méthode interne pour créer les tables SQL au premier démarrage
    void initialiserBaseDeDonnees();

public:
    Supermarche();  // Le constructeur ouvre la base
    ~Supermarche(); // Le destructeur ferme la base (Crucial !)

    void initialiser(int nbCaisses);
    
    void ajouterClient(const Client& client); 
    void servirClient(int numeroCaisse);
    void ouvrirCaisse(int numero);
    void fermerCaisse(int numero);
    void viderCaisse(int numero);

    std::vector<Caisse>& getCaisses();
    int getTotalClientsServis() const;
    
    // --- NOUVEAU : GETTER FINANCIER ---
    double getChiffreAffairesTotal() const; 

    // --- GESTION DU CATALOGUE (SQLITE) ---
    std::vector<Produit> getCatalogue(); 
    void ajouterProduit(const Produit& p);
    void supprimerProduit(int id);
    void modifierPrixProduit(int id, double nouveauPrix);
};