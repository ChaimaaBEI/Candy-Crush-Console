#pragma once
#include "Matrice.h"

/* =========================================================================
   Affichage
   -------------------------------------------------------------------------
   But :
   - Afficher la matrice de jeu en console avec un graphisme lisible.

   Paramètre :
   - pMatrice : adresse de la matrice à afficher.
               Si pMatrice == NULL, la fonction ne fait rien.
   ========================================================================= */
void Affichage(etMatrice* pMatrice);