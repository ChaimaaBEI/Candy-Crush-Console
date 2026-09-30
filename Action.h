#pragma once

/* =========================================================================
   Structure Action
   -------------------------------------------------------------------------
   Représente une action à stocker dans la file.
   - nomAction : nom de l'action (max 19 caractères + '\0')
   - x1, y1    : coordonnées du premier pion
   - x2, y2    : coordonnées du second pion
   ========================================================================= */
struct etAction
{
	char nomAction[20];
	int x1;
	int y1;
	int x2;
	int y2;
};