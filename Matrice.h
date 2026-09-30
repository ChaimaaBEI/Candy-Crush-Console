#pragma once
#include "Case.h"
#include "Action.h"
#include "Queue.h"

/* =========================================================================
   Matrice.h
   -------------------------------------------------------------------------
   Ce fichier contient :
   1) Les constantes de dimensions de la grille
   2) Le type etMatrice (tableau 2D de cases)
   3) Le prototype de la fonction d'initialisation
   ========================================================================= */

   /* =========================================================================
	  Dimensions de la matrice de jeu
	  -------------------------------------------------------------------------
	  TAILLE1 : nombre de lignes
	  TAILLE2 : nombre de colonnes
	  ========================================================================= */
#define TAILLE1 20
#define TAILLE2 20

	/* =========================================================================
	   Structure etMatrice
	   -------------------------------------------------------------------------
	   Représente la grille de jeu complète.
	   Chaque cellule est une etCase, qui contient :
		- la couleur du pion
		- la présence/absence de gélatine
	========================================================================= */
struct etMatrice
{
	etCase tMatrice[TAILLE1][TAILLE2];
};

	/* =========================================================================
	   InitialisationMatrice
       -------------------------------------------------------------------------
       But :
        - Initialiser toute la matrice de jeu.

       Paramètres :
        - pMatrice      : adresse de la matrice à initialiser
        - NbrGelatine : nombre de cases qui doivent contenir de la gélatine
    ========================================================================= */
void InitialisationMatrice(etMatrice* pMatrice, int NbrGelatine, int* pRC);

/* =========================================================================
   Codes de retour pour Deplacement
   -------------------------------------------------------------------------
   DEPLACEMENT_OK  (1) : les deux cases sont adjacentes et leur couleur
                         a été intervertie avec succès
   DEPLACEMENT_ERR (0) : déplacement impossible — pointeur invalide
                         ou cases non adjacentes
   ========================================================================= */
#define DEPLACEMENT_OK  1
#define DEPLACEMENT_ERR 0

/* =========================================================================
   Deplacement
   -------------------------------------------------------------------------
   But :
    - Intervertir deux cases adjacentes choisies par le joueur.

   Paramètres :
    - pMatrice  : adresse de la matrice à modifier
    - stAction  : structure contenant les coordonnées des deux cases
                  (x1,y1) = première case
                  (x2,y2) = deuxième case
    - pRC       : adresse du code de retour
                  DEPLACEMENT_OK  (1) : déplacement effectué
                  DEPLACEMENT_ERR (0) : cases non adjacentes ou pointeur invalide

   Remarque :
    - Les deux cases doivent être adjacentes horizontalement ou verticalement
    - Le déplacement en diagonale est interdit
    - La gélatine ne bouge pas, seule la couleur du pion est intervertie
   ========================================================================= */
void Deplacement(etMatrice* pMatrice, etAction stAction, int* pRC);

/* =========================================================================
   Codes de retour pour SuppressionV3
   -------------------------------------------------------------------------
   SUPPRESSION_OK  (1) : suppression effectuée avec succès
   SUPPRESSION_ERR (0) : pointeur invalide ou pions non alignés
   ========================================================================= */
#define SUPPRESSION_OK  1
#define SUPPRESSION_ERR 0

/* =========================================================================
   SuppressionV3
   -------------------------------------------------------------------------
   But :
	- Supprimer 3 pions alignés verticalement dans une colonne.
	- Les pions situés au-dessus descendent de 3 cases.
	- 3 nouveaux pions aléatoires entrent par le haut de la colonne.
	- Si une des cases supprimées contient de la gélatine, elle est supprimée.

   Paramètres :
	- pMatrice  : adresse de la matrice à modifier
	- stAction  : structure contenant les coordonnées des 3 pions alignés
				  (x1,y1) = coordonnées du premier pion (le plus haut)
				  (x2,y2) = coordonnées du dernier pion (le plus bas)
	- pRC       : adresse du code de retour
				  SUPPRESSION_OK  (1) : suppression effectuée avec succès
				  SUPPRESSION_ERR (0) : pointeur invalide ou pions non alignés
   ========================================================================= */
void SuppressionV3(etMatrice* pMatrice, etAction stAction, int* pRC);

/* =========================================================================
   SuppressionH3
   -------------------------------------------------------------------------
   But :
	- Supprimer 3 pions alignés horizontalement dans une ligne.
	- Les pions situés au-dessus de chaque colonne concernée descendent d'une ligne.
	- 3 nouveaux pions aléatoires entrent par le haut de chaque colonne concernée.
	- Si une des cases supprimées contient de la gélatine, elle est supprimée.

   Paramètres :
	- pMatrice  : adresse de la matrice à modifier
	- stAction  : structure contenant les coordonnées des 3 pions alignés
				  (x1,y1) = coordonnées du premier pion (le plus à gauche)
				  (x2,y2) = coordonnées du dernier pion (le plus à droite)
	- pRC       : adresse du code de retour
				  SUPPRESSION_OK  (1) : suppression effectuée avec succès
				  SUPPRESSION_ERR (0) : pointeur invalide ou pions non alignés
   ========================================================================= */
void SuppressionH3(etMatrice* pMatrice, etAction stAction, int* pRC);

/* =========================================================================
   SuppressionV4
   -------------------------------------------------------------------------
   But :
	- Supprimer toute une colonne de la matrice.
	- 10 nouveaux pions aléatoires entrent par le haut de la colonne.
	- Si une des cases supprimées contient de la gélatine, elle est supprimée.
	- La gélatine ne bouge pas.

   Paramètres :
	- pMatrice  : adresse de la matrice à modifier
	- stAction  : structure contenant les coordonnées de la colonne à supprimer
				  y1 = numéro de la colonne à supprimer
	- pRC       : adresse du code de retour
				  SUPPRESSION_OK  (1) : suppression effectuée avec succès
				  SUPPRESSION_ERR (0) : pointeur invalide
   ========================================================================= */
void SuppressionV4(etMatrice* pMatrice, etAction stAction, int* pRC);

/* =========================================================================
   SuppressionH4
   -------------------------------------------------------------------------
   But :
	- Supprimer toute une ligne de la matrice.
	- Les pions situés au-dessus descendent d'une ligne.
	- 10 nouveaux pions aléatoires entrent par la première ligne.
	- Si une des cases supprimées contient de la gélatine, elle est supprimée.
	- La gélatine ne bouge pas.

   Paramètres :
	- pMatrice  : adresse de la matrice à modifier
	- stAction  : structure contenant les coordonnées de la ligne à supprimer
				  x1 = numéro de la ligne à supprimer
	- pRC       : adresse du code de retour
				  SUPPRESSION_OK  (1) : suppression effectuée avec succès
				  SUPPRESSION_ERR (0) : pointeur invalide
   ========================================================================= */
void SuppressionH4(etMatrice* pMatrice, etAction stAction, int* pRC);

/* =========================================================================
   Codes de retour pour Verification
   -------------------------------------------------------------------------
   VERIFICATION_OUI (1) : la permutation des deux pions provoque bien
						  un alignement de 3 ou 4 pions de même couleur
						  en horizontal ou en vertical
   VERIFICATION_NON (0) : la permutation ne provoque aucun alignement,
						  ou le pointeur est invalide
   ========================================================================= */
#define VERIFICATION_OUI 1
#define VERIFICATION_NON 0

/* =========================================================================
   Verification
   -------------------------------------------------------------------------
   But :
	- Vérifier que la permutation de deux pions adjacents va provoquer
	  un alignement de 3 ou 4 pions de même couleur en horizontal ou vertical.

   Paramètres :
	- pMatrice : adresse de la matrice à analyser
	- x1, y1   : coordonnées du premier pion
	- x2, y2   : coordonnées du second pion
	- pRC      : adresse du code de retour
				 VERIFICATION_OUI (1) : la permutation provoque un alignement
				 VERIFICATION_NON (0) : pas d'alignement ou pointeur invalide
   ========================================================================= */
void Verification(etMatrice* pMatrice, int x1, int y1, int x2, int y2, int* pRC);

/* =========================================================================
   Codes de retour pour Calcul
   -------------------------------------------------------------------------
   CALCUL_OK  (1) : analyse de la grille effectuée avec succès,
					les actions détectées ont été ajoutées dans la Queue
   CALCUL_ERR (0) : pointeur invalide, aucune analyse effectuée
   ========================================================================= */
#define CALCUL_OK  1
#define CALCUL_ERR 0

/* =========================================================================
   Calcul
   -------------------------------------------------------------------------
   But :
	- Parcourir la grille et détecter les séries de 3 ou 4 pions identiques
	  alignés horizontalement ou verticalement.
	- Pour chaque série détectée, créer une action et l'ajouter dans la Queue.

   Paramètres :
	- pMatrice : adresse de la matrice à analyser
	- pQueue   : adresse de la file dans laquelle ajouter les actions
	- pRC      : adresse du code de retour
				 CALCUL_OK  (1) : analyse effectuée avec succès
				 CALCUL_ERR (0) : pointeur invalide
   ========================================================================= */
void Calcul(etMatrice* pMatrice, etQueue* pQueue, int* pRC);