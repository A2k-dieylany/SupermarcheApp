#include "serveur.h"
#include "../lib/httplib.h"
#include "json_helper.h"
#include <iostream>
#include <mutex> // La bibliothèque pour le Thread-Safety
#include <vector>
#include <string>

void Serveur::demarrer(Supermarche& sm) {
    httplib::Server srv;
    
    // Le cadenas global de notre API
    std::mutex api_mutex; 

    // 1. LECTURE : Renvoyer l'état complet du supermarché
    srv.Get("/api/etat", [&](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(api_mutex); 
        
        json j = supermarcheToJson(sm);
        res.set_content(j.dump(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    // 2. ACTION : Ajouter un client (AVEC PANIER)
    srv.Post("/api/client/ajouter", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(api_mutex); 
        
        try {
            json body = json::parse(req.body);
            
            // --- CORRECTION DU TYPAGE ---
            std::string nomClient = body["nom"].get<std::string>();
            Client nouveauClient(nomClient);
            
            std::vector<int> produitsIds = body["produitsIds"].get<std::vector<int>>();
            
            for (int id : produitsIds) {
                for (auto p : sm.getCatalogue()) { 
                    if (p.getId() == id) {
                        nouveauClient.ajouterProduit(p);
                        break;
                    }
                }
            }
            
            sm.ajouterClient(nouveauClient);
            
            res.set_content(supermarcheToJson(sm).dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            res.status = 400; 
            res.set_content(json{{"erreur", e.what()}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    // 3. ACTION : Servir le prochain client d'une caisse
    srv.Post("/api/caisse/servir", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(api_mutex); 
        
        try {
            json body = json::parse(req.body);
            int numero = body["numero"].get<int>(); // CORRECTION TYPAGE
            sm.servirClient(numero);
            
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
        std::lock_guard<std::mutex> lock(api_mutex); 
        
         try {
            json body = json::parse(req.body);
            int numero = body["numero"].get<int>(); // CORRECTION TYPAGE
            sm.ouvrirCaisse(numero);
            
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
        std::lock_guard<std::mutex> lock(api_mutex); 
        
         try {
            json body = json::parse(req.body);
            int numero = body["numero"].get<int>(); // CORRECTION TYPAGE
            sm.fermerCaisse(numero);
            
            res.set_content(supermarcheToJson(sm).dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"erreur", e.what()}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    // 7. ACTION ADMIN : Ajouter un produit
    srv.Post("/api/produit/ajouter", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(api_mutex); 
        
        try {
            json body = json::parse(req.body);
            
            // CORRECTION TYPAGE
            int id = body["id"].get<int>();
            std::string nom = body["nom"].get<std::string>();
            double prix = body["prix"].get<double>();
            std::string cat = body["categorie"].get<std::string>();
            
            Produit nouveau(id, nom, prix, cat);
            sm.ajouterProduit(nouveau);
            
            res.set_content(supermarcheToJson(sm).dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"erreur", e.what()}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    // 8. ACTION ADMIN : Supprimer un produit
    srv.Post("/api/produit/supprimer", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(api_mutex); 
        
        try {
            json body = json::parse(req.body);
            int id = body["id"].get<int>(); // CORRECTION TYPAGE
            sm.supprimerProduit(id);
            
            res.set_content(supermarcheToJson(sm).dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"erreur", e.what()}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    // 9. ACTION ADMIN : Modifier le prix
    srv.Post("/api/produit/modifier", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(api_mutex);
        
        try {
            json body = json::parse(req.body);
            int id = body["id"].get<int>();
            double prix = body["prix"].get<double>();
            
            sm.modifierPrixProduit(id, prix);
            
            res.set_content(supermarcheToJson(sm).dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"erreur", e.what()}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        }
    });

    // --- ROUTES POUR L'HISTORIQUE ---
    srv.Post("/api/historique/ajouter", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(api_mutex);
        try {
            json body = json::parse(req.body);
            std::string client = body["client"].get<std::string>();
            double total = body["total"].get<double>();
            
            sm.enregistrerTicket(client, total);
            
            res.set_content(json{{"status", "ok"}}.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch(...) {
            res.status = 500;
        }
    });

  srv.Get("/api/historique", [&](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(api_mutex);
        
        // CORRIGÉ : Plus de .dump() ici, car c'est déjà un string !
        res.set_content(sm.getHistoriqueTickets(), "application/json"); 
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    srv.set_mount_point("/", "./web");
    
    std::cout << "Serveur demarre sur http://localhost:8080" << std::endl;
    std::cout << "[SYSTEME] Thread-Safety actif (Mutex Lock global)" << std::endl;
    std::cout << "Appuyez sur Ctrl+C pour arreter." << std::endl;

    srv.listen("0.0.0.0", 8080);
}