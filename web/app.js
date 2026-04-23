const API = 'http://localhost:8080/api';

// --- VARIABLES GLOBALES ---
let panierCourant = []; 
let totalPanier = 0.0;
let intervalSimulation = null; // Variable pour retenir l'état de la simulation

// --- SYSTEME DE NOTIFICATIONS ---
function showToast(message, type = 'success') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    const icon = type === 'success' ? '✅' : '❌';
    toast.innerHTML = `<span>${icon}</span> <span>${message}</span>`;
    container.appendChild(toast);
    setTimeout(() => {
        toast.style.animation = 'fadeOut 0.3s ease forwards';
        setTimeout(() => toast.remove(), 300);
    }, 3000);
}

// --- RAFRAICHISSEMENT GENERAL ---
async function rafraichir() {
    try {
        const res = await fetch(`${API}/etat`);
        if (!res.ok) throw new Error("Erreur serveur");
        const data = await res.json();
        
        document.getElementById('totalServis').textContent = data.totalServis;
        afficherCaisses(data.caisses);
        if (data.catalogue) afficherCatalogue(data.catalogue);
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
        carte.style.animationDelay = `${index * 0.1}s`;

        if (c.express) carte.classList.add('express');
        if (!c.ouverte) carte.classList.add('fermee');
        if (c.nbClients >= 5) carte.classList.add('surcharge');

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

// --- CATALOGUE ET PANIER ---
function getIconeCategorie(categorie) {
    const icones = { 'Boulangerie': '🥖', 'Frais': '🥛', 'Épicerie': '🥫', 'Hygiène': '🧼', 'Surgelé': '❄️', 'Boisson': '🥤', 'Fruits': '🍎', 'Légumes': '🥦' };
    return icones[categorie] || '📦'; 
}

function afficherCatalogue(catalogue) {
    const zone = document.getElementById('catalogue-rayons');
    if (!zone || zone.children.length > 0) return; 
    zone.innerHTML = ''; 

    catalogue.forEach(p => {
        const card = document.createElement('div');
        card.className = 'produit-card';
        const icone = getIconeCategorie(p.categorie);
        
        card.innerHTML = `
            <div class="produit-icon">${icone}</div>
            <div class="produit-nom">${p.nom}</div>
            <div class="produit-prix">${p.prix.toFixed(2)} FCFA</div>
        `;
        
        card.onclick = () => {
            ajouterAuPanier(p.id, p.nom, p.prix);
            card.style.transform = 'scale(0.9)';
            setTimeout(() => card.style.transform = '', 100);
        };
        zone.appendChild(card);
    });
}

function ajouterAuPanier(id, nom, prix) {
    panierCourant.push({ id: id, nom: nom, prix: prix });
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
            return false; 
        }

        if (msgSucces) showToast(msgSucces, 'success'); 
        rafraichir();
        return true; 
    } catch (erreur) {
        showToast("Erreur de connexion au serveur.", 'error');
        return false;
    }
}

// --- ACTIONS DES CAISSES ---
async function ajouterClient() {
    const nom = document.getElementById('nomClient').value || "Client Anonyme";
    if (panierCourant.length === 0) return showToast("Le panier est vide ! Ajoutez des articles d'abord.", "error");

    const idsPourServeur = panierCourant.map(produit => produit.id);
    const succes = await envoyerAction('/client/ajouter', { nom: nom, produitsIds: idsPourServeur }, `${nom} a rejoint une caisse.`);
    
    if (succes) {
        imprimerTicket(nom, panierCourant, totalPanier);
        document.getElementById('nomClient').value = ''; 
        viderPanier();
    }
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
    envoyerAction('/caisse/fermer', { numero: num }, null); // Pas de message de succès forcé ici
}
function imprimerTicket(nomClient, listeProduits, total) {
    const ticketModal = document.getElementById('ticket-modal');
    const lignesZone = document.getElementById('ticket-lignes');
    if (!ticketModal || !lignesZone) return; 
    
    lignesZone.innerHTML = `<div style="text-align: center; margin-bottom: 10px; color: #666;">Client : ${nomClient}</div>`;
    listeProduits.forEach(p => {
        lignesZone.innerHTML += `<div class="ticket-item"><span>1x ${p.nom}</span><span>${p.prix.toFixed(2)} FCFA</span></div>`;
    });
    document.getElementById('ticket-total-prix').textContent = `${total.toFixed(2)} FCFA`;
    ticketModal.classList.add('visible');
    
    // --- LA LIGNE MAGIQUE QUI MANQUAIT ---
    // On envoie discrètement le ticket au serveur C++ pour la comptabilité
    fetch(`${API}/historique/ajouter`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ client: nomClient, total: total })
    });

    setTimeout(() => ticketModal.classList.remove('visible'), 4000); 
}
// --- BONUS : MOTEUR DE SIMULATION IA ---
function toggleSimulation() {
    const btn = document.getElementById('btn-simu');
    
    if (intervalSimulation) {
        clearInterval(intervalSimulation);
        intervalSimulation = null;
        btn.innerHTML = "🤖 Activer Simulation";
        btn.style.background = "transparent";
        btn.style.color = "#8B5CF6";
        showToast("Simulation arrêtée", "success");
    } else {
        btn.innerHTML = "🛑 Stopper Simulation";
        btn.style.background = "#8B5CF6";
        btn.style.color = "white";
        showToast("Mode Autopilot activé !", "success");
        
        intervalSimulation = setInterval(() => {
            if (Math.random() > 0.3) {
                genererClientAleatoire();
            } else {
                servirClientAutomatique(); 
            }
        }, 2000); 
    }
}

function genererClientAleatoire() {
    const prenoms = ["Djily", "Awa", "Moussa", "Fatou", "Cheikh", "Ndeye", "Amadou"];
    const nomAleatoire = prenoms[Math.floor(Math.random() * prenoms.length)];
    const nbProduits = Math.floor(Math.random() * 4) + 1;
    const itemsPossibles = document.querySelectorAll('.produit-card');
    
    if (itemsPossibles.length === 0) return; 

    for(let i = 0; i < nbProduits; i++) {
        const itemAleatoire = itemsPossibles[Math.floor(Math.random() * itemsPossibles.length)];
        itemAleatoire.click(); 
    }

    document.getElementById('nomClient').value = nomAleatoire + " (Auto)";
    ajouterClient();
}

async function servirClientAutomatique() {
    try {
        const res = await fetch(`${API}/etat`);
        const data = await res.json();
        
        const caissesOccupees = data.caisses.filter(c => c.ouverte && c.nbClients > 0);
        if (caissesOccupees.length > 0) {
            const caisseCible = caissesOccupees.reduce((max, c) => c.nbClients > max.nbClients ? c : max);
            envoyerAction('/caisse/servir', { numero: caisseCible.numero }, `🤖 (Auto) Client encaissé en caisse ${caisseCible.numero} !`);
        }
    } catch (e) {
        console.log("Erreur dans la simulation automatique");
    }
}

// Boucle principale
setInterval(rafraichir, 2000);
rafraichir();