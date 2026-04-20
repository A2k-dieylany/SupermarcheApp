#pragma once
#include "../lib/json.hpp"
#include "supermarche.h"

// On utilise un alias pour que le code soit plus lisible
using json = nlohmann::json;

// Traduit une seule caisse en JSON
inline json caisseToJson(const Caisse& c) {
    return {
        {"numero", c.getNumero()},
        {"ouverte", c.estOuverte()},
        {"express", c.isExpress()},
        {"nbClients", c.getTailleFile()},
        {"tempsAttente", c.getTempsAttente()}
    };
}

// Traduit tout le supermarché (statistiques + liste des caisses)
inline json supermarcheToJson(Supermarche& sm) {
    json j;
    j["totalServis"] = sm.getTotalClientsServis();
    j["caisses"] = json::array(); // On prépare un tableau vide
    
    // On boucle sur toutes les caisses du supermarché
    for (const auto& c : sm.getCaisses()) {
        j["caisses"].push_back(caisseToJson(c)); // On ajoute chaque caisse traduite
    }
    
    return j;
}