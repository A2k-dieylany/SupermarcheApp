#pragma once
#include <vector>
#include <string>
#include <sqlite3.h>
#include "caisse.h"
#include "produit.h"

class Supermarche {
private:
    std::vector<Caisse> caisses;
    int totalClientsServis = 0;
    double chiffreAffairesTotal = 0.0; 
    sqlite3* db; 

    void initialiserBaseDeDonnees();

public:
    Supermarche();
    ~Supermarche();

    void initialiser(int nbCaisses);
    void ajouterClient(const Client& client); 
    void servirClient(int numeroCaisse);
    void ouvrirCaisse(int numero);
    void fermerCaisse(int numero);
    void viderCaisse(int numero);
    
    // --- HISTORIQUE (CORRIGÉ) ---
    void enregistrerTicket(const std::string& nomClient, double total);
    // On renvoie un string (texte) au lieu d'un JSON pour éviter les crashs d'inclusion
    std::string getHistoriqueTickets(); 

    std::vector<Caisse>& getCaisses();
    int getTotalClientsServis() const;
    double getChiffreAffairesTotal() const; 

    std::vector<Produit> getCatalogue(); 
    void ajouterProduit(const Produit& p);
    void supprimerProduit(int id);
    void modifierPrixProduit(int id, double nouveauPrix);
};