#include "serveur.h"
#include "../lib/httplib.h"
#include "json_helper.h"
#include <iostream>

void Serveur::demarrer(Supermarche& sm) {
    httplib::Server srv;

    // 1. LECTURE : Renvoyer l'état complet du supermarché
    srv.Get("/api/etat", [&](const httplib::Request&, httplib::Response& res) {
        json j = supermarcheToJson(sm);
        res.set_content(j.dump(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    // 2. ACTION : Ajouter un client
    srv.Post("/api/client/ajouter", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            // On extrait les valeurs du JSON envoyé par le client
            sm.ajouterClient(body["nom"], body["nbArticles"]);
            
            res.set_content(supermarcheToJson(sm).dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            res.status = 400; // Code d'erreur HTTP (Bad Request)
            res.set_content(json{{"erreur", e.what()}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    // 3. ACTION : Servir le prochain client d'une caisse
    srv.Post("/api/caisse/servir", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            sm.servirClient(body["numero"]);
            
            res.set_content(supermarcheToJson(sm).dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"erreur", e.what()}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    // 4. ACTION : Ouvrir une caisse
    srv.Post("/api/caisse/ouvrir", [&](const httplib::Request& req, httplib::Response& res) {
         try {
            json body = json::parse(req.body);
            sm.ouvrirCaisse(body["numero"]);
            
            res.set_content(supermarcheToJson(sm).dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"erreur", e.what()}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    // 5. ACTION : Fermer une caisse
    srv.Post("/api/caisse/fermer", [&](const httplib::Request& req, httplib::Response& res) {
         try {
            json body = json::parse(req.body);
            sm.fermerCaisse(body["numero"]);
            
            res.set_content(supermarcheToJson(sm).dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"erreur", e.what()}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    // Indique au serveur où trouver nos futurs fichiers HTML/JS de l'interface (Phase 3)
    srv.set_mount_point("/", "./web");

    std::cout << "Serveur demarre sur http://localhost:8080" << std::endl;
    std::cout << "Appuyez sur Ctrl+C pour arreter." << std::endl;
    
    // Le serveur écoute en boucle
    srv.listen("0.0.0.0", 8080);
}