#pragma once
#include <vector>
#include "caisse.h"
#include "produit.h"

class Supermarche {
private:
    std::vector<Caisse> caisses;
    std::vector<Produit> catalogue; // Le magasin a maintenant des rayons !
    int totalClientsServis = 0;

public:
    Supermarche(); // Nouveau constructeur
    void initialiser(int nbCaisses);
    
    // On ne passe plus un nom et un nombre, on passe directement un objet Client complet
    void ajouterClient(const Client& client); 
    
    void servirClient(int numeroCaisse);
    void ouvrirCaisse(int numero);
    void fermerCaisse(int numero);
    void viderCaisse(int numero);

    std::vector<Caisse>& getCaisses();
    int getTotalClientsServis() const;
    const std::vector<Produit>& getCatalogue() const; // Pour envoyer les produits au site web
    // Gestion de l'inventaire
    void ajouterProduit(const Produit& p);
    void supprimerProduit(int id);
};