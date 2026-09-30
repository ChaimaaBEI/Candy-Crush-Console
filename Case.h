#pragma once

/* =========================================================================
   Case.h
   -------------------------------------------------------------------------
   Ce fichier définit le type "case" de la grille de jeu.
   Chaque case contient :
   - une couleur de pion
   - une information de présence/absence de gélatine
   ========================================================================= */

   /* =========================================================================
	  enum etCouleursCase
	  -------------------------------------------------------------------------
	  Représente les couleurs possibles d'un pion.
	  - Valeurs implicites :
		Jaune = 0, Vert = 1, Bleu = 2, Rouge = 3, Mauve = 4
	  ========================================================================= */
enum etCouleursCase
{
	Jaune,
	Vert,
	Bleu,
	Rouge,
	Mauve
};

/* =========================================================================
   struct etCase
   -------------------------------------------------------------------------
   Représente une case de la matrice de jeu :
   - enCouleur      : couleur du pion sur la case
   - AvecGelatine  : indicateur de gélatine (1 = oui, 0 = non)
   ========================================================================= */
struct etCase
{
	enum etCouleursCase enCouleur;
	int AvecGelatine;
};