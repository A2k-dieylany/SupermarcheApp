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

// --- CHARGEMENT DES DONNÉES ET DU DASHBOARD ---
async function chargerCatalogue() {
    try {
        const res = await fetch(`${API}/etat`);
        if (!res.ok) throw new Error("Erreur serveur");
        
        const data = await res.json();
        
        // 1. Mise à jour du catalogue
        if (data.catalogue) {
            afficherTableauProduits(data.catalogue);
        }

        // 2. Mise à jour du Dashboard Financier
        animerCompteur('dash-clients', data.totalServis, 0); 
        
        const ca = data.chiffreAffaires || 0;
        animerCompteur('dash-ca', ca, 2);
        
        const panierMoyen = data.totalServis > 0 ? (ca / data.totalServis) : 0;
        animerCompteur('dash-moyen', panierMoyen, 2);

    } catch (erreur) {
        console.error("Erreur de connexion", erreur);
    }
}

// Fonction UX : Fait défiler les nombres de manière fluide pour le "Waouh effect"
function animerCompteur(elementId, valeurCible, decimales) {
    const obj = document.getElementById(elementId);
    if (!obj) return;
    
    const valeurActuelle = parseFloat(obj.textContent) || 0;
    if (valeurActuelle === valeurCible) return;

    let etape = 0;
    const maxEtapes = 20;
    const increment = (valeurCible - valeurActuelle) / maxEtapes;

    const timer = setInterval(() => {
        etape++;
        let valeurTemporaire = valeurActuelle + (increment * etape);
        obj.textContent = valeurTemporaire.toFixed(decimales);
        
        if (etape >= maxEtapes) {
            clearInterval(timer);
            obj.textContent = valeurCible.toFixed(decimales); 
        }
    }, 20);
}

// --- NOUVEAU : AFFICHAGE DU CATALOGUE AVEC EDITION DES PRIX ---
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
        ligne.style.cssText = `
            display: flex; justify-content: space-between; align-items: center;
            padding: 12px 15px; background: #F8FAFC; border: 1px solid var(--border, #E5E7EB);
            border-radius: 10px; flex-wrap: wrap; gap: 10px;
        `;
        
        ligne.innerHTML = `
            <div style="display: flex; gap: 15px; align-items: center; flex: 1; min-width: 200px;">
                <span style="font-weight: 800; color: var(--primary, #2563EB); width: 40px;">#${p.id}</span>
                <span style="font-weight: 600; color: var(--text-900, #111827); flex: 1;">${p.nom}</span>
                <span style="color: var(--text-600, #4B5563); font-size: 0.9em; width: 100px;">${p.categorie}</span>
            </div>
            
            <div style="display: flex; gap: 10px; align-items: center;">
                <input type="number" id="prix-edit-${p.id}" value="${p.prix.toFixed(2)}" step="50" style="width: 100px; padding: 6px; border-radius: 5px; border: 1px solid #D1D5DB; text-align: right; font-weight: bold; color: var(--primary, #2563EB);">
                <span style="font-weight: 700; font-size: 0.9em; width: 40px;">FCFA</span>
                
                <button style="padding: 6px 12px; font-size: 0.8rem; background: #10B981; color: white; border: none; border-radius: 5px; cursor: pointer; font-weight: bold;" onclick="modifierPrix(${p.id})">
                    Valider
                </button>

                <button class="btn-outline-danger" style="padding: 6px 12px; font-size: 0.8rem;" onclick="supprimerProduit(${p.id})">
                    Supprimer
                </button>
            </div>
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

// --- NOUVEAU : FONCTION DE MISE A JOUR DU PRIX ---
async function modifierPrix(id) {
    const input = document.getElementById(`prix-edit-${id}`);
    const nouveauPrix = parseFloat(input.value);

    if (isNaN(nouveauPrix) || nouveauPrix < 0) {
        return showToast("Le prix entré est invalide.", "error");
    }

    // On utilise la route API C++ créée précédemment
    executerActionAdmin('/produit/modifier', { 
        id: id, prix: nouveauPrix 
    }, `Le prix du produit #${id} a été mis à jour à ${nouveauPrix} FCFA !`);
}

// On charge le catalogue au démarrage
chargerCatalogue();

// On ajoute un rafraîchissement global (seulement les compteurs) toutes les 5 secondes
// pour voir l'argent rentrer sans gêner la saisie des prix !
setInterval(async () => {
    try {
        const res = await fetch(`${API}/etat`);
        const data = await res.json();
        
        const ca = data.chiffreAffaires || 0;
        animerCompteur('dash-clients', data.totalServis, 0); 
        animerCompteur('dash-ca', ca, 2);
        const panierMoyen = data.totalServis > 0 ? (ca / data.totalServis) : 0;
        animerCompteur('dash-moyen', panierMoyen, 2);
    } catch(e) {}
}, 5000);