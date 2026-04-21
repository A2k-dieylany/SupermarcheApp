#pragma once
#include "../lib/json.hpp"
#include "supermarche.h"

using json = nlohmann::json;

// NOUVEAU : Traduit un Produit en JSON
inline json produitToJson(const Produit& p) {
    return {
        {"id", p.getId()},
        {"nom", p.getNom()},
        {"prix", p.getPrix()},
        {"categorie", p.getCategorie()}
    };
}

inline json caisseToJson(const Caisse& c) {
    return {
        {"numero", c.getNumero()},
        {"ouverte", c.estOuverte()},
        {"express", c.isExpress()},
        {"nbClients", c.getTailleFile()},
        {"tempsAttente", c.getTempsAttente()}
    };
}

inline json supermarcheToJson(Supermarche& sm) {
    json j;
    j["totalServis"] = sm.getTotalClientsServis();
    
    j["caisses"] = json::array();
    for (const auto& c : sm.getCaisses()) {
        j["caisses"].push_back(caisseToJson(c));
    }
    
    // NOUVEAU : On ajoute le catalogue à la réponse globale !
    j["catalogue"] = json::array();
    for (const auto& p : sm.getCatalogue()) {
        j["catalogue"].push_back(produitToJson(p));
    }
    
    return j;
}