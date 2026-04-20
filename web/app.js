// L'adresse de notre serveur C++
const API = 'http://localhost:8080/api';

// --------------------------------------------------------
// 1. FONCTIONS DE LECTURE (GET)
// --------------------------------------------------------

// Récupère l'état du supermarché et met à jour l'affichage
async function rafraichir() {
    try {
        const res = await fetch(`${API}/etat`);
        if (!res.ok) throw new Error("Erreur serveur");
        
        const data = await res.json();
        
        // Met à jour les stats
        document.getElementById('totalServis').textContent = data.totalServis;
        
        // Dessine les caisses
        afficherCaisses(data.caisses);
    } catch (erreur) {
        console.error("Impossible de joindre le serveur:", erreur);
    }
}

// Fonction utilitaire pour générer le HTML de chaque caisse
function afficherCaisses(caisses) {
    const grille = document.getElementById('grille-caisses');
    grille.innerHTML = ''; // On vide la grille avant de la redessiner

    caisses.forEach(c => {
        const carte = document.createElement('div');
        
        // On assigne les classes CSS selon l'état de la caisse
        carte.className = 'carte-caisse';
        if (c.express) carte.classList.add('express');
        if (!c.ouverte) carte.classList.add('fermee');

        const statutTexte = c.ouverte ? 'Ouverte' : 'Fermée';
        const statutClasse = c.ouverte ? 'ouvert' : 'ferme';
        const titreExpress = c.express ? ' (Express)' : '';

        // On injecte le contenu HTML
        carte.innerHTML = `
            <h3>Caisse N° ${c.numero} ${titreExpress}</h3>
            <p><span class="statut ${statutClasse}">${statutTexte}</span></p>
            <p>👥 Clients en attente : <strong>${c.nbClients}</strong></p>
            <p>⏱️ Temps d'attente estimé : <strong>${c.tempsAttente}s</strong></p>
        `;
        grille.appendChild(carte);
    });
}

// --------------------------------------------------------
// 2. FONCTIONS D'ACTION (POST)
// --------------------------------------------------------

async function envoyerAction(route, payload) {
    try {
        const res = await fetch(`${API}${route}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        // Si le serveur C++ a renvoyé une erreur (ex: fermer une caisse non vide)
        if (!res.ok) {
            const dataErreur = await res.json();
            alert("Erreur métier : " + dataErreur.erreur);
            return;
        }

        // Si c'est un succès, on rafraîchit l'écran immédiatement
        rafraichir();
    } catch (erreur) {
        alert("Erreur de communication avec le serveur.");
    }
}

// Les fonctions attachées aux boutons
function ajouterClient() {
    const nom = document.getElementById('nomClient').value || "Client Anonyme";
    const nb = parseInt(document.getElementById('nbArticles').value) || 1;
    envoyerAction('/client/ajouter', { nom: nom, nbArticles: nb });
}

function servirClient() {
    const num = parseInt(document.getElementById('numeroCaisse').value);
    if (!num) return alert("Veuillez entrer un numéro de caisse");
    envoyerAction('/caisse/servir', { numero: num });
}

function ouvrirCaisse() {
    const num = parseInt(document.getElementById('numeroCaisse').value);
    if (!num) return alert("Veuillez entrer un numéro de caisse");
    envoyerAction('/caisse/ouvrir', { numero: num });
}

function fermerCaisse() {
    const num = parseInt(document.getElementById('numeroCaisse').value);
    if (!num) return alert("Veuillez entrer un numéro de caisse");
    envoyerAction('/caisse/fermer', { numero: num });
}

// --------------------------------------------------------
// 3. INITIALISATION
// --------------------------------------------------------

// On rafraîchit l'interface toutes les 2 secondes (Temps Réel)
setInterval(rafraichir, 2000);

// Premier chargement immédiat au démarrage de la page
rafraichir();