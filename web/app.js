const API = 'http://localhost:8080/api';

// --- VARIABLES GLOBALES DU PANIER (NOUVEAU) ---
let panierCourant = []; // Stocke les identifiants (IDs) des produits
let totalPanier = 0.0;

// --- SYSTEME DE NOTIFICATIONS (TOASTS) ---
function showToast(message, type = 'success') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    
    const icon = type === 'success' ? '✅' : '❌';
    toast.innerHTML = `<span>${icon}</span> <span>${message}</span>`;
    
    container.appendChild(toast);

    // Détruit la notification après 3 secondes
    setTimeout(() => {
        toast.style.animation = 'fadeOut 0.3s ease forwards';
        setTimeout(() => toast.remove(), 300);
    }, 3000);
}

// --- LOGIQUE PRINCIPALE ---
async function rafraichir() {
    try {
        const res = await fetch(`${API}/etat`);
        if (!res.ok) throw new Error("Erreur serveur");
        
        const data = await res.json();
        
        document.getElementById('totalServis').textContent = data.totalServis;
        afficherCaisses(data.caisses);
        
        // NOUVEAU : Si le serveur envoie un catalogue, on crée les rayons
        if (data.catalogue) {
            afficherCatalogue(data.catalogue);
        }
    } catch (erreur) {
        console.error("Impossible de joindre le serveur");
    }
}

function afficherCaisses(caisses) {
    const grille = document.getElementById('grille-caisses');
    grille.innerHTML = '';

    caisses.forEach((c, index) => {
        const carte = document.createElement('div');
        carte.className = 'carte-caisse';
        
        // Délai d'animation en cascade pour un effet stylé
        carte.style.animationDelay = `${index * 0.1}s`;

        if (c.express) carte.classList.add('express');
        if (!c.ouverte) carte.classList.add('fermee');

        const statutTexte = c.ouverte ? 'Ouverte' : 'Fermée';
        const statutClasse = c.ouverte ? 'ouvert' : 'ferme';
        const iconeExpress = c.express ? '⚡' : '🛒';

        carte.innerHTML = `
            <h3>Caisse N° ${c.numero} <span class="statut ${statutClasse}">${statutTexte}</span></h3>
            <p><strong>${iconeExpress}</strong> ${c.express ? 'File Express' : 'File Normale'}</p>
            <p>👥 Clients en file : <strong>${c.nbClients}</strong></p>
            <p>⏱️ Temps d'attente : <strong>${c.tempsAttente}s</strong></p>
        `;
        grille.appendChild(carte);
    });
}

// --- NOUVEAU : GESTION DU CATALOGUE ET DU PANIER ---
function afficherCatalogue(catalogue) {
    const zone = document.getElementById('catalogue-rayons');
    if (!zone) return; 
    
    // CORRECTION : On vérifie s'il y a déjà des boutons, pas si le texte est vide
    if (zone.children.length > 0) return; 

    // On efface les commentaires HTML invisibles
    zone.innerHTML = ''; 

    catalogue.forEach(p => {
        const btn = document.createElement('button');
        btn.className = 'btn-outline';
        // Design du bouton produit
        btn.innerHTML = `+ ${p.nom} <span style="opacity:0.6; font-size:0.8em; margin-left:5px;">(${p.prix.toFixed(2)}€)</span>`;
        // Action au clic : on ajoute l'ID et le prix au panier local
        btn.onclick = () => ajouterAuPanier(p.id, p.prix);
        zone.appendChild(btn);
    });
}


function ajouterAuPanier(id, prix) {
    panierCourant.push(id);
    totalPanier += prix;
    mettreAJourPanierUI();
}

function viderPanier() {
    panierCourant = [];
    totalPanier = 0.0;
    mettreAJourPanierUI();
}

function mettreAJourPanierUI() {
    document.getElementById('panier-count').textContent = panierCourant.length;
    document.getElementById('panier-total').textContent = totalPanier.toFixed(2);
}

// --- ACTIONS VERS L'API ---
async function envoyerAction(route, payload, msgSucces) {
    try {
        const res = await fetch(`${API}${route}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (!res.ok) {
            const dataErreur = await res.json();
            showToast(dataErreur.erreur, 'error'); 
            return;
        }

        showToast(msgSucces, 'success'); 
        rafraichir();
    } catch (erreur) {
        showToast("Erreur de connexion au serveur.", 'error');
    }
}

// NOUVEAU : Ajouter un client avec son panier complet
function ajouterClient() {
    const nom = document.getElementById('nomClient').value || "Client Anonyme";

    // Règle métier : On ne passe pas en caisse avec un panier vide
    if (panierCourant.length === 0) {
        return showToast("Le panier est vide ! Ajoutez des articles d'abord.", "error");
    }

    // On envoie le JSON complexe attendu par le serveur C++
    envoyerAction('/client/ajouter', { 
        nom: nom, 
        produitsIds: panierCourant 
    }, `${nom} a rejoint une caisse (Total: ${totalPanier.toFixed(2)}€)`);
    
    // On nettoie l'interface pour le prochain client
    document.getElementById('nomClient').value = ''; 
    viderPanier();
}

function servirClient() {
    const num = parseInt(document.getElementById('numeroCaisse').value);
    if (!num) return showToast("Veuillez entrer un numéro de caisse", 'error');
    envoyerAction('/caisse/servir', { numero: num }, `Client de la caisse ${num} servi !`);
}

function ouvrirCaisse() {
    const num = parseInt(document.getElementById('numeroCaisse').value);
    if (!num) return showToast("Veuillez entrer un numéro de caisse", 'error');
    envoyerAction('/caisse/ouvrir', { numero: num }, `Caisse ${num} ouverte.`);
}

function fermerCaisse() {
    const num = parseInt(document.getElementById('numeroCaisse').value);
    if (!num) return showToast("Veuillez entrer un numéro de caisse", 'error');
    envoyerAction('/caisse/fermer', { numero: num }, `Caisse ${num} fermée.`);
}

// Boucle de rafraîchissement toutes les 2 secondes
setInterval(rafraichir, 2000);
rafraichir();

