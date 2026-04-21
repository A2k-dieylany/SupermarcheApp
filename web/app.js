const API = 'http://localhost:8080/api';

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

// --- ACTIONS ---
async function envoyerAction(route, payload, msgSucces) {
    try {
        const res = await fetch(`${API}${route}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (!res.ok) {
            const dataErreur = await res.json();
            showToast(dataErreur.erreur, 'error'); // Affiche une belle notification rouge
            return;
        }

        showToast(msgSucces, 'success'); // Affiche une belle notification verte
        rafraichir();
    } catch (erreur) {
        showToast("Erreur de connexion au serveur.", 'error');
    }
}

function ajouterClient() {
    const nom = document.getElementById('nomClient').value || "Client Anonyme";
    const nb = parseInt(document.getElementById('nbArticles').value) || 1;
    envoyerAction('/client/ajouter', { nom: nom, nbArticles: nb }, `${nom} a été ajouté(e) avec succès !`);
    
    // Vide le champ nom après ajout
    document.getElementById('nomClient').value = ''; 
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

setInterval(rafraichir, 2000);
rafraichir();