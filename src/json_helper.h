#pragma once
#include "../lib/json.hpp"
#include "supermarche.h"

using json = nlohmann::json;

// Traduit un Produit en JSON
inline json produitToJson(const Produit& p) {
    return {
        {"id", p.getId()},
        {"nom", p.getNom()},
        {"prix", p.getPrix()},
        {"categorie", p.getCategorie()}
    };
}

// Traduit une Caisse en JSON
inline json caisseToJson(const Caisse& c) {
    return {
        {"numero", c.getNumero()},
        {"ouverte", c.estOuverte()},
        {"express", c.isExpress()},
        {"nbClients", c.getTailleFile()},
        {"tempsAttente", c.getTempsAttente()}
    };
}

// Traduit l'état global du supermarché en JSON
inline json supermarcheToJson(Supermarche& sm) {
    json j;
    j["totalServis"] = sm.getTotalClientsServis();
    
    j["caisses"] = json::array();
    for (const auto& c : sm.getCaisses()) {
        j["caisses"].push_back(caisseToJson(c));
    }
    
    j["catalogue"] = json::array();
    
    // CORRECTION : On utilise "auto p" au lieu de "const auto& p" 
    // car getCatalogue() génère un vecteur tout neuf depuis la BDD SQLite.
    for (auto p : sm.getCatalogue()) {
        j["catalogue"].push_back(produitToJson(p));
    }
    
    return j;
}