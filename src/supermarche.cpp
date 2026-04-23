#include "supermarche.h"
#include "json_helper.h" // <-- On l'inclut ici, en toute sécurité !
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <string>

// --- INITIALISATION & PERSISTANCE (SQLITE) ---
Supermarche::Supermarche() {
    if (sqlite3_open("supermarche.db", &db)) {
        throw std::runtime_error("Erreur fatale : Impossible d'ouvrir la base de données !");
    }
    initialiserBaseDeDonnees();
}

Supermarche::~Supermarche() {
    sqlite3_close(db);
}

void Supermarche::initialiserBaseDeDonnees() {
    char* messageErreur = nullptr;
    const char* sql = "CREATE TABLE IF NOT EXISTS produits (id INTEGER PRIMARY KEY, nom TEXT NOT NULL, prix REAL NOT NULL, categorie TEXT NOT NULL);";
    if (sqlite3_exec(db, sql, nullptr, nullptr, &messageErreur) != SQLITE_OK) {
        std::string err(messageErreur);
        sqlite3_free(messageErreur);
        throw std::runtime_error("Erreur SQL : " + err);
    }
    if (getCatalogue().empty()) {
        ajouterProduit(Produit(1, "Baguette", 150.0, "Boulangerie"));
        ajouterProduit(Produit(2, "Lait de vache", 800.0, "Frais"));
        ajouterProduit(Produit(3, "Riz (1kg)", 1500.0, "Épicerie"));
        ajouterProduit(Produit(4, "Café moulu", 2500.0, "Épicerie"));
        ajouterProduit(Produit(5, "Savon", 500.0, "Hygiène"));
    }
}

void Supermarche::initialiser(int nbCaisses) {
    caisses.clear();
    for (int i = 1; i <= nbCaisses; i++) caisses.push_back(Caisse(i, i == 1)); 
}

// --- LOGIQUE MÉTIER ---
void Supermarche::ajouterClient(const Client& client) {
    if (caisses.empty()) throw std::runtime_error("Erreur : Aucune caisse installée.");
    int nbArticles = client.getNbArticles();
    Caisse* caisseChoisie = nullptr;
    
    if (nbArticles <= 10) {
        for (auto& c : caisses) {
            if (c.estOuverte() && c.isExpress()) {
                if (!caisseChoisie || c.getTailleFile() < caisseChoisie->getTailleFile()) caisseChoisie = &c;
            }
        }
    }
    if (!caisseChoisie) {
        for (auto& c : caisses) {
            if (c.estOuverte() && !c.isExpress()) {
                if (!caisseChoisie || c.getTailleFile() < caisseChoisie->getTailleFile()) caisseChoisie = &c;
            }
        }
    }
    if (caisseChoisie) caisseChoisie->ajouterClient(client);
    else throw std::runtime_error("Toutes les caisses sont fermées.");
}

void Supermarche::servirClient(int numeroCaisse) {
    for (auto& c : caisses) {
        if (c.getNumero() == numeroCaisse) {
            double montantEncaisse = c.servirClient(); 
            chiffreAffairesTotal += montantEncaisse;
            totalClientsServis++;
            return;
        }
    }
    throw std::runtime_error("Caisse introuvable.");
}

void Supermarche::ouvrirCaisse(int numero) {
    for (auto& c : caisses) {
        if (c.getNumero() == numero) { c.ouvrir(); return; }
    }
}

void Supermarche::fermerCaisse(int numero) {
    Caisse* caisseCible = nullptr;
    for (auto& c : caisses) {
        if (c.getNumero() == numero) { caisseCible = &c; break; }
    }
    if (!caisseCible) throw std::runtime_error("Caisse introuvable.");

    bool autreCaisseOuverte = false;
    for (auto& c : caisses) {
        if (c.getNumero() != numero && c.estOuverte()) { autreCaisseOuverte = true; break; }
    }

    if (!autreCaisseOuverte && caisseCible->getTailleFile() > 0) {
        throw std::runtime_error("Sécurité : Impossible de fermer la dernière caisse, des clients attendent !");
    }

    std::queue<Client> aRedistribuer = caisseCible->fermer();
    while (!aRedistribuer.empty()) {
        Client orphelin = aRedistribuer.front();
        aRedistribuer.pop();
        ajouterClient(orphelin); 
    }
}

void Supermarche::viderCaisse(int numero) {
    for (auto& c : caisses) {
        if (c.getNumero() == numero) { c.vider(); return; }
    }
}

// --- GESTION DU CATALOGUE ---
std::vector<Produit> Supermarche::getCatalogue() {
    std::vector<Produit> liste;
    const char* sql = "SELECT id, nom, prix, categorie FROM produits ORDER BY categorie ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string nom = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            double prix = sqlite3_column_double(stmt, 2);
            std::string cat = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            liste.push_back(Produit(id, nom, prix, cat));
        }
        sqlite3_finalize(stmt);
    }
    return liste;
}

void Supermarche::ajouterProduit(const Produit& p) {
    const char* sql = "INSERT INTO produits (id, nom, prix, categorie) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, p.getId());
        sqlite3_bind_text(stmt, 2, p.getNom().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 3, p.getPrix());
        sqlite3_bind_text(stmt, 4, p.getCategorie().c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("L'ID produit existe déjà en base.");
        }
        sqlite3_finalize(stmt);
    }
}

void Supermarche::supprimerProduit(int id) {
    const char* sql = "DELETE FROM produits WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

std::vector<Caisse>& Supermarche::getCaisses() { return caisses; }
int Supermarche::getTotalClientsServis() const { return totalClientsServis; }
double Supermarche::getChiffreAffairesTotal() const { return chiffreAffairesTotal; }

void Supermarche::modifierPrixProduit(int id, double nouveauPrix) {
    const char* sql = "UPDATE produits SET prix = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, nouveauPrix);
        sqlite3_bind_int(stmt, 2, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    } else {
        throw std::runtime_error("Erreur base de données lors de la modification.");
    }
}

// --- GESTION DE L'HISTORIQUE ---
void Supermarche::enregistrerTicket(const std::string& nomClient, double total) {
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS historique (id INTEGER PRIMARY KEY AUTOINCREMENT, client TEXT, total REAL, date_heure DATETIME DEFAULT (datetime('now', 'localtime')));", nullptr, nullptr, nullptr);
    const char* sql = "INSERT INTO historique (client, total) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nomClient.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 2, total);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

std::string Supermarche::getHistoriqueTickets() {
    json liste = json::array();
    const char* sql = "SELECT id, client, total, date_heure FROM historique ORDER BY id DESC LIMIT 50;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json ticket;
            ticket["id"] = sqlite3_column_int(stmt, 0);
            ticket["client"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            ticket["total"] = sqlite3_column_double(stmt, 2);
            ticket["date"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            liste.push_back(ticket);
        }
        sqlite3_finalize(stmt);
    }
    return liste.dump(); // On transforme le JSON en texte sécurisé
}