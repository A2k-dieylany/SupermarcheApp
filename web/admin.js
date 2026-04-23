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

// --- ANIMATION DES COMPTEURS ---
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

// ==========================================
// PARTIE 1 : GESTION DU CATALOGUE (TON CODE)
// ==========================================

async function chargerCatalogue() {
    try {
        const res = await fetch(`${API}/etat`);
        if (!res.ok) throw new Error("Erreur serveur");
        const data = await res.json();
        
        if (data.catalogue) {
            afficherTableauProduits(data.catalogue);
        }

        const ca = data.chiffreAffaires || 0;
        animerCompteur('dash-clients', data.totalServis, 0); 
        animerCompteur('dash-ca', ca, 2);
        const panierMoyen = data.totalServis > 0 ? (ca / data.totalServis) : 0;
        animerCompteur('dash-moyen', panierMoyen, 2);
    } catch (erreur) {
        console.error("Erreur de connexion", erreur);
    }
}

function afficherTableauProduits(catalogue) {
    const zone = document.getElementById('liste-produits-admin');
    if (!zone) return;
    
    zone.innerHTML = ''; 

    if (catalogue.length === 0) {
        zone.innerHTML = `<p style="color: var(--text-400);">Le catalogue est vide.</p>`;
        return;
    }

    catalogue.forEach(p => {
        const ligne = document.createElement('div');
        ligne.style.cssText = `
            display: flex; justify-content: space-between; align-items: center;
            padding: 12px 15px; background: #F8FAFC; border: 1px solid var(--border, #E5E7EB);
            border-radius: 10px; flex-wrap: wrap; gap: 10px; margin-bottom: 8px;
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

                <button class="btn-outline-danger" style="padding: 6px 12px; font-size: 0.8rem; border: 1px solid #EF4444; color: #EF4444; background: transparent; border-radius: 5px; cursor: pointer;" onclick="supprimerProduit(${p.id})">
                    Supprimer
                </button>
            </div>
        `;
        zone.appendChild(ligne);
    });
}

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
        chargerCatalogue(); 
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

    executerActionAdmin('/produit/ajouter', { id: id, nom: nom, prix: prix, categorie: categorie }, `Le produit ${nom} a été ajouté !`);

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

async function modifierPrix(id) {
    const input = document.getElementById(`prix-edit-${id}`);
    const nouveauPrix = parseFloat(input.value);

    if (isNaN(nouveauPrix) || nouveauPrix < 0) {
        return showToast("Le prix entré est invalide.", "error");
    }

    executerActionAdmin('/produit/modifier', { id: id, prix: nouveauPrix }, `Le prix du produit #${id} a été mis à jour à ${nouveauPrix} FCFA !`);
}


// ==========================================
// PARTIE 2 : TEMPS RÉEL (GRAPHIQUE & HISTORIQUE)
// ==========================================

let caChartInstance = null;
const historiqueTemps = [];
const historiqueCA = [];

function initialiserGraphique() {
    const ctx = document.getElementById('caChart');
    if (!ctx) return;

    caChartInstance = new Chart(ctx, {
        type: 'line',
        data: {
            labels: historiqueTemps,
            datasets: [{
                label: "Chiffre d'Affaires (FCFA)",
                data: historiqueCA,
                borderColor: '#10B981',
                backgroundColor: 'rgba(16, 185, 129, 0.2)',
                borderWidth: 3,
                pointBackgroundColor: '#059669',
                pointRadius: 4,
                tension: 0.4,
                fill: true
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            animation: { duration: 0 },
            scales: {
                x: { grid: { display: false } },
                y: { beginAtZero: true }
            },
            plugins: { legend: { display: false } }
        }
    });
}

// Fonction propre et isolée pour l'historique
async function chargerHistorique() {
    try {
        const res = await fetch(`${API}/historique`);
        const tickets = await res.json();
        
        const zone = document.getElementById('liste-historique');
        if (!zone) return;
        
        zone.innerHTML = '';
        
        if (tickets.length === 0) {
            zone.innerHTML = `<p style="color: #9CA3AF; font-style: italic;">Aucune vente enregistrée pour le moment.</p>`;
            return;
        }

        tickets.forEach(t => {
            zone.innerHTML += `
                <div style="display: flex; justify-content: space-between; padding: 10px; background: #F3F4F6; border-radius: 8px; font-size: 0.9em; margin-bottom: 5px;">
                    <div>
                        <span style="color: #6B7280; margin-right: 15px;">🕒 ${t.date}</span>
                        <span style="font-weight: 600; color: #1F2937;">${t.client}</span>
                    </div>
                    <div style="font-weight: 800; color: #10B981;">
                        + ${t.total.toFixed(2)} FCFA
                    </div>
                </div>
            `;
        });
    } catch (e) { 
        console.error("Erreur historique", e); 
    }
}

// La boucle qui tourne toutes les 3 secondes sans casser la saisie dans le catalogue
async function boucleSurveillanceTempsReel() {
    try {
        const res = await fetch(`${API}/etat`);
        const data = await res.json();
        
        const ca = data.chiffreAffaires || 0;
        animerCompteur('dash-clients', data.totalServis, 0); 
        animerCompteur('dash-ca', ca, 2);
        const panierMoyen = data.totalServis > 0 ? (ca / data.totalServis) : 0;
        animerCompteur('dash-moyen', panierMoyen, 2);

        if (caChartInstance) {
            const now = new Date();
            const heure = now.getHours() + ':' + String(now.getMinutes()).padStart(2, '0') + ':' + String(now.getSeconds()).padStart(2, '0');

            historiqueTemps.push(heure);
            historiqueCA.push(ca);

            if (historiqueTemps.length > 15) {
                historiqueTemps.shift();
                historiqueCA.shift();
            }
            caChartInstance.update();
        }

        // On actualise l'historique
        chargerHistorique();

    } catch(e) {
        console.error("Erreur boucle temps réel:", e);
    }
}

// ==========================================
// DÉMARRAGE DE LA PAGE
// ==========================================
document.addEventListener('DOMContentLoaded', () => {
    chargerCatalogue();      // Charge les produits modifiables
    initialiserGraphique();  // Prépare le graphique
    chargerHistorique();     // Charge la liste des tickets
    
    // Lance l'actualisation financière toutes les 3 secondes
    setInterval(boucleSurveillanceTempsReel, 3000); 
});