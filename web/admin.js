const API = 'http://localhost:8080/api';

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

// --- CHARGEMENT DU CATALOGUE ---
async function chargerCatalogue() {
    try {
        // On réutilise la route /etat car elle contient tout le catalogue
        const res = await fetch(`${API}/etat`);
        if (!res.ok) throw new Error("Erreur serveur");
        
        const data = await res.json();
        if (data.catalogue) {
            afficherTableauProduits(data.catalogue);
        }
    } catch (erreur) {
        console.error("Erreur de connexion", erreur);
    }
}

function afficherTableauProduits(catalogue) {
    const zone = document.getElementById('liste-produits-admin');
    if (!zone) return;
    
    zone.innerHTML = ''; // On vide avant de redessiner

    if (catalogue.length === 0) {
        zone.innerHTML = `<p style="color: var(--text-400);">Le catalogue est vide.</p>`;
        return;
    }

    catalogue.forEach(p => {
        const ligne = document.createElement('div');
        // On réutilise quelques variables CSS pour faire une belle ligne
        ligne.style.cssText = `
            display: flex; justify-content: space-between; align-items: center;
            padding: 12px 15px; background: #F8FAFC; border: 1px solid var(--border);
            border-radius: 10px;
        `;
        
        ligne.innerHTML = `
            <div style="display: flex; gap: 15px; align-items: center; flex-wrap: wrap;">
                <span style="font-weight: 800; color: var(--primary); width: 40px;">#${p.id}</span>
                <span style="font-weight: 600; color: var(--text-900); width: 150px;">${p.nom}</span>
                <span style="color: var(--text-600); font-size: 0.9em; width: 100px;">${p.categorie}</span>
                <span style="font-weight: 700; width: 80px;">${p.prix.toFixed(2)} €</span>
            </div>
            <button class="btn-outline-danger" style="padding: 6px 12px; font-size: 0.8rem;" onclick="supprimerProduit(${p.id})">
                Supprimer
            </button>
        `;
        zone.appendChild(ligne);
    });
}

// --- ACTIONS ADMINISTRATEUR ---
async function executerActionAdmin(route, payload, msgSucces) {
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
        chargerCatalogue(); // On rafraîchit la liste immédiatement après l'action
    } catch (erreur) {
        showToast("Erreur de connexion au serveur.", 'error');
    }
}

function ajouterNouveauProduit() {
    const id = parseInt(document.getElementById('prodId').value);
    const nom = document.getElementById('prodNom').value;
    const prix = parseFloat(document.getElementById('prodPrix').value);
    const categorie = document.getElementById('prodCat').value;

    if (!id || !nom || isNaN(prix) || !categorie) {
        return showToast("Veuillez remplir tous les champs correctement.", "error");
    }

    executerActionAdmin('/produit/ajouter', { 
        id: id, nom: nom, prix: prix, categorie: categorie 
    }, `Le produit ${nom} a été ajouté au catalogue !`);

    // On vide le formulaire
    document.getElementById('prodId').value = '';
    document.getElementById('prodNom').value = '';
    document.getElementById('prodPrix').value = '';
    document.getElementById('prodCat').value = '';
}

function supprimerProduit(id) {
    if (confirm(`Êtes-vous sûr de vouloir supprimer le produit #${id} ?`)) {
        executerActionAdmin('/produit/supprimer', { id: id }, `Le produit #${id} a été supprimé.`);
    }
}

// On charge le catalogue au démarrage
chargerCatalogue();