#include <iostream>
#include "supermarche.h"

int main() {
    std::cout << "--- TEST FINAL PHASE 1 : SUPERMARCHE ---" << std::endl;
    
    Supermarche sm;
    
    // Initialise 1 caisse express (ouverte) + 2 caisses ordinaires (fermées)
    sm.initialiser(2); 
    
    // On ouvre les caisses ordinaires (numéros 2 et 3)
    sm.ouvrirCaisse(2);
    sm.ouvrirCaisse(3);
    
    std::cout << "\n--- Arrivee des clients ---" << std::endl;
    // Doit aller en express (<= 10 articles)
    sm.ajouterClient("Awa", 5); 
    
    // Doit aller en caisse 2 (la première trouvée vide)
    sm.ajouterClient("Moussa", 15); 
    
    // Doit aller en caisse 3 (car la 2 a déjà un client)
    sm.ajouterClient("Fatou", 20); 
    
    // Doit aller en caisse 2 (car toutes ont 1 client, retour à la première)
    sm.ajouterClient("Omar", 45); 
    
    std::cout << "\n--- Etat des files ---" << std::endl;
    for (const auto& c : sm.getCaisses()) {
        std::cout << "Caisse " << c.getNumero() 
                  << (c.isExpress() ? " (Express)" : " (Normale)") 
                  << " : " << c.getTailleFile() << " clients en attente." << std::endl;
    }
    
    std::cout << "\n--- Service ---" << std::endl;
    sm.servirClient(2);
    std::cout << "Total de clients servis dans le supermarche : " << sm.getTotalClientsServis() << std::endl;
    
    return 0;
}