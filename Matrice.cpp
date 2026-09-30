#include <stdlib.h>
#include "Matrice.h"
#include <cstring>

#ifndef matrice202205
#define matrice202205

#define INIT_OK  1
#define INIT_ERR 0

/* =============================================================================================================
   InitialisationMatrice
   -------------------------------------------------------------------------------------------------------------
   But :
	- Initialiser la matrice de jeu.
	- Chaque case reçoit :
		1) Une couleur de pion choisie aléatoirement
		2) Une indication de présence ou absence de gélatine

   Paramètres :
	- pMatrice    : adresse de la matrice à initialiser
	- NbrGelatine : nombre de cases qui doivent contenir de la gélatine
	- pRC         : adresse du code de retour
					INIT_OK  (1) : initialisation réussie
					INIT_ERR (0) : pointeur invalide, rien n'a été fait

   Méthode :
	- Étape 1 : vérifier que le pointeur est valide
	- Étape 2 : borner NbrGelatine entre 0 et le nombre total de cases
	- Étape 3 et 4 : initialiser toutes les cases (couleur aléatoire, sans gélatine)
					 ET remplir le tableau d'indices linéaires en une seule boucle
	- Étape 5 : mélanger aléatoirement le tableau d'indices (Fisher-Yates)
	- Étape 6 : tirer aléatoirement NbrGelatine cases distinctes et y placer la gélatine
   ============================================================================================================ */
void InitialisationMatrice(etMatrice* pMatrice, int NbrGelatine, int* pRC)
{
	int Ligne;
	int Colonne;
	int NombreTotalCases;
	int Indices[TAILLE1 * TAILLE2];
	int i;
	int j;
	int tmp;

	if (pMatrice != 0)
	{
		NombreTotalCases = TAILLE1 * TAILLE2;

		/* On s'assure que NbrGelatine est compris entre 0 et le nombre total de cases. */
		if (NbrGelatine < 0) {
			NbrGelatine = 0;
		}
		else if (NbrGelatine > NombreTotalCases) {
			NbrGelatine = NombreTotalCases;
		}

		/* -----------------------------------------------------------------------
		   Étape 3 et 4 : Initialisation de toutes les cases de la grille
		   ET préparation du tableau d'indices linéaires.
		   On parcourt chaque case en utilisant un index linéaire i.
		   Chaque case reçoit :
			- une couleur aléatoire parmi les 5 couleurs disponibles
			- aucune gélatine (sera placée à l'étape 6)
		   Simultanément, on remplit le tableau d'indices avec les valeurs 0 à NombreTotalCases - 1.
		   Chaque case de la grille est identifiée par un index unique :
			index = Ligne * TAILLE2 + Colonne
		   ----------------------------------------------------------------------- */
		for (i = 0; i < NombreTotalCases; i++)
		{
			/* Conversion de l'index linéaire en coordonnées (Ligne, Colonne) */
			Ligne = i / TAILLE2;
			Colonne = i % TAILLE2;
			/* Couleur tirée aléatoirement parmi les 5 valeurs de l'enum */
			pMatrice->tMatrice[Ligne][Colonne].enCouleur = (etCouleursCase)(rand() % 5);
			/* Pas de gélatine par défaut */
			pMatrice->tMatrice[Ligne][Colonne].AvecGelatine = 0;
			/* Remplissage du tableau d'indices en même temps */
			Indices[i] = i;
		}

		/* -----------------------------------------------------------------------
		   Étape 5 : Mélange aléatoire du tableau d'indices (Fisher-Yates).
		   On parcourt le tableau de la fin vers le début.
		   À chaque étape, on échange l'élément courant avec un élément
		   tiré aléatoirement parmi les éléments restants.
		   Résultat : un ordre aléatoire garanti, sans répétition.
		   ----------------------------------------------------------------------- */
		for (i = NombreTotalCases - 1; i > 0; i--)
		{
			/* On tire un index aléatoire entre 0 et i inclus */
			j = rand() % (i + 1);
			/* On échange Indices[i] et Indices[j] */
			tmp = Indices[i];
			Indices[i] = Indices[j];
			Indices[j] = tmp;
		}

		/* -----------------------------------------------------------------------
		   Étape 6 : Placement de la gélatine sur NbrGelatine cases distinctes.
		   On prend les NbrGelatine premiers indices du tableau mélangé.
		   On retrouve la ligne et la colonne à partir de l'index linéaire :
			Ligne   = index / TAILLE2
			Colonne = index % TAILLE2
		   ----------------------------------------------------------------------- */
		for (i = 0; i < NbrGelatine; i++)
		{
			/* Conversion de l'index linéaire en coordonnées (Ligne, Colonne) */
			Ligne = Indices[i] / TAILLE2;
			Colonne = Indices[i] % TAILLE2;
			/* Placement de la gélatine sur la case tirée */
			pMatrice->tMatrice[Ligne][Colonne].AvecGelatine = 1;
		}

		*pRC = INIT_OK;
	}
	else
	{
		/* Pointeur invalide : on signale l'erreur sans toucher à la mémoire. */
		*pRC = INIT_ERR;
	}
}

/* =============================================================================================================
   Deplacement
   -------------------------------------------------------------------------------------------------------------
   But :
	- Intervertir la couleur de deux cases adjacentes choisies par le joueur.

   Paramètres :
	- pMatrice  : adresse de la matrice à modifier
	- stAction  : structure contenant les coordonnées des deux cases
				  (x1,y1) = première case
				  (x2,y2) = deuxième case
	- pRC       : adresse du code de retour
				  DEPLACEMENT_OK  (1) : déplacement effectué avec succès
				  DEPLACEMENT_ERR (0) : cases non adjacentes ou pointeur invalide

   Méthode :
	- Étape 1 : vérifier que le pointeur est valide
	- Étape 2 : calculer la différence de ligne et de colonne entre les deux cases
	- Étape 3 : vérifier que les cases sont adjacentes (1 seule case d'écart en X ou en Y)
	- Étape 4 : intervertir uniquement la couleur des deux cases

   Remarque :
	- Le déplacement en diagonale est interdit
	- La gélatine ne bouge pas, seule la couleur du pion est intervertie
	- Les coordonnées doivent être comprises entre 0 et TAILLE1-1 / TAILLE2-1
   ============================================================================================================ */
void Deplacement(etMatrice* pMatrice, etAction stAction, int* pRC)
{
	etCase stCaseTemp;
	int dDiffX;
	int dDiffY;

	if (pMatrice != 0)
	{
		/* -----------------------------------------------------------------------
		   Vérification que les coordonnées sont dans les bornes de la grille.
		   Si une coordonnée est hors grille, on signale l'erreur.
		   ----------------------------------------------------------------------- */
		if (stAction.x1 < 0 || stAction.x1 >= TAILLE1 ||
			stAction.y1 < 0 || stAction.y1 >= TAILLE2 ||
			stAction.x2 < 0 || stAction.x2 >= TAILLE1 ||
			stAction.y2 < 0 || stAction.y2 >= TAILLE2)
		{
			*pRC = DEPLACEMENT_ERR;
		}
		else
		{
			/* -----------------------------------------------------------------------
			   Étape 2 : Calcul de la différence entre les coordonnées des deux cases.
			   On prend la valeur absolue pour éviter les valeurs négatives.
			   ----------------------------------------------------------------------- */
			dDiffX = stAction.x1 - stAction.x2;
			if (dDiffX < 0)
			{
				dDiffX = -dDiffX;
			}

			dDiffY = stAction.y1 - stAction.y2;
			if (dDiffY < 0)
			{
				dDiffY = -dDiffY;
			}

			/* -----------------------------------------------------------------------
			   Étape 3 : Vérification de l'adjacence.
			   Les cases sont adjacentes si elles ne diffèrent que d'une case
			   en X ou en Y, mais pas les deux en même temps.
			   ----------------------------------------------------------------------- */
			if ((dDiffX == 1 && dDiffY == 0) || (dDiffX == 0 && dDiffY == 1))
			{
				/* -----------------------------------------------------------------------
				   Étape 4 : Interversion des deux cases complètes.
				   On utilise une variable temporaire pour ne pas perdre le contenu
				   de la première case lors de l'échange :
					- On sauvegarde la case (x1,y1) dans stCaseTemp
					- On écrase la case (x1,y1) par la case (x2,y2)
					- On donne à (x2,y2) le contenu sauvegardé dans stCaseTemp
				   ----------------------------------------------------------------------- */
				stCaseTemp = pMatrice->tMatrice[stAction.x1][stAction.y1];
				pMatrice->tMatrice[stAction.x1][stAction.y1] = pMatrice->tMatrice[stAction.x2][stAction.y2];
				pMatrice->tMatrice[stAction.x2][stAction.y2] = stCaseTemp;

				*pRC = DEPLACEMENT_OK;
			}
			else
			{
				/* Cases non adjacentes ou diagonale : déplacement refusé */
				*pRC = DEPLACEMENT_ERR;
			}
		}
	}
	else
	{
		/* Pointeur invalide : on signale l'erreur sans toucher à la mémoire */
		*pRC = DEPLACEMENT_ERR;
	}
}

/* =============================================================================================================
   SuppressionV3
   -------------------------------------------------------------------------------------------------------------
   But :
	- Supprimer 3 pions alignés verticalement dans une colonne.
	- Les pions situés au-dessus descendent de 3 cases.
	- 3 nouveaux pions aléatoires entrent par le haut de la colonne.
	- Si une des cases supprimées contient de la gélatine, elle est supprimée.
	- La gélatine ne bouge pas.

   Paramètres :
	- pMatrice  : adresse de la matrice à modifier
	- stAction  : structure contenant les coordonnées des 3 pions alignés
				  (x1,y1) = coordonnées du premier pion (le plus haut)
				  (x2,y2) = coordonnées du dernier pion (le plus bas)
	- pRC       : adresse du code de retour
				  SUPPRESSION_OK  (1) : suppression effectuée avec succès
				  SUPPRESSION_ERR (0) : pointeur invalide, coordonnées hors
										grille ou pions non alignés

   Méthode :
	- Étape 1 : vérifier que le pointeur est valide
	- Étape 2 : vérifier que les coordonnées sont dans les bornes de la grille
	- Étape 3 : vérifier que les 3 pions sont bien alignés verticalement
	- Étape 4 : supprimer la gélatine sur les 3 cases concernées
	- Étape 5 : faire descendre les pions du dessus de 3 cases
	- Étape 6 : faire entrer 3 nouveaux pions aléatoires par le haut
   ============================================================================================================ */
void SuppressionV3(etMatrice* pMatrice, etAction stAction, int* pRC)
{
	int Ligne;
	int i;

	if (pMatrice != 0)
	{
		/* -----------------------------------------------------------------------
		   Étape 2 : Vérification que les coordonnées sont dans les bornes.
		   ----------------------------------------------------------------------- */
		if (stAction.x1 < 0 || stAction.x1 >= TAILLE1 ||
			stAction.y1 < 0 || stAction.y1 >= TAILLE2 ||
			stAction.x2 < 0 || stAction.x2 >= TAILLE1 ||
			stAction.y2 < 0 || stAction.y2 >= TAILLE2)
		{
			*pRC = SUPPRESSION_ERR;
		}
		else
		{
			/* -----------------------------------------------------------------------
			   Étape 3 : Vérification de l'alignement vertical.
			   Les 3 pions doivent être dans la même colonne (y1 == y2)
			   et sur 3 lignes consécutives (x2 == x1 + 2).
			   ----------------------------------------------------------------------- */
			if (stAction.y1 == stAction.y2 && stAction.x2 == stAction.x1 + 2)
			{
				/* -----------------------------------------------------------------------
				   Étape 4 : Si une des 3 cases supprimées contient de la gélatine,
				   on la retire en mettant AvecGelatine à 0.
				   La gélatine disparait avec le pion qui se trouvait dessus.
				   ----------------------------------------------------------------------- */
				for (i = stAction.x1; i <= stAction.x2; i++)
				{
					pMatrice->tMatrice[i][stAction.y1].AvecGelatine = 0;
				}

				/* -----------------------------------------------------------------------
				   Étape 5 : Descente des pions du dessus de 3 cases.
				   On parcourt la colonne de bas en haut en partant de la dernière
				   case supprimée (x2) jusqu'à la ligne 3.
				   Chaque case prend la couleur de la case située 3 lignes au-dessus
				   d'elle — ce qui simule la chute des pions vers le bas.
				   Exemple : la case ligne 5 prend la couleur de la case ligne 2.
				   ----------------------------------------------------------------------- */
				for (Ligne = stAction.x2; Ligne >= 3; Ligne--)
				{
					pMatrice->tMatrice[Ligne][stAction.y1].enCouleur =
						pMatrice->tMatrice[Ligne - 3][stAction.y1].enCouleur;
				}

				/* -----------------------------------------------------------------------
				   Étape 6 : Les 3 cases libérées en haut de la colonne (lignes 0, 1, 2)
				   reçoivent chacune une nouvelle couleur tirée aléatoirement parmi
				   les 5 couleurs disponibles.
				   ----------------------------------------------------------------------- */
				for (i = 0; i < 3; i++)
				{
					pMatrice->tMatrice[i][stAction.y1].enCouleur = (etCouleursCase)(rand() % 5);
				}

				*pRC = SUPPRESSION_OK;
			}
			else
			{
				/* Pions non alignés verticalement : suppression refusée */
				*pRC = SUPPRESSION_ERR;
			}
		}
	}
	else
	{
		/* Pointeur invalide : on signale l'erreur sans toucher à la mémoire */
		*pRC = SUPPRESSION_ERR;
	}
}

/* =============================================================================================================
   SuppressionH3
   -------------------------------------------------------------------------------------------------------------
   But :
	- Supprimer 3 pions alignés horizontalement dans une ligne.
	- Les pions situés au-dessus de chaque colonne concernée descendent d'une ligne.
	- 3 nouveaux pions aléatoires entrent par le haut de chaque colonne concernée.
	- Si une des cases supprimées contient de la gélatine, elle est supprimée.
	- La gélatine ne bouge pas.

   Paramètres :
	- pMatrice  : adresse de la matrice à modifier
	- stAction  : structure contenant les coordonnées des 3 pions alignés
				  (x1,y1) = coordonnées du premier pion (le plus à gauche)
				  (x2,y2) = coordonnées du dernier pion (le plus à droite)
	- pRC       : adresse du code de retour
				  SUPPRESSION_OK  (1) : suppression effectuée avec succès
				  SUPPRESSION_ERR (0) : pointeur invalide, coordonnées hors
										grille ou pions non alignés

   Méthode :
	- Étape 1 : vérifier que le pointeur est valide
	- Étape 2 : vérifier que les coordonnées sont dans les bornes de la grille
	- Étape 3 : vérifier que les 3 pions sont bien alignés horizontalement
	- Étape 4 : supprimer la gélatine sur les 3 cases concernées
	- Étape 5 : faire descendre les pions du dessus d'une ligne pour chaque colonne concernée
	- Étape 6 : faire entrer un nouveau pion aléatoire par le haut de chaque colonne concernée
   ============================================================================================================ */
void SuppressionH3(etMatrice* pMatrice, etAction stAction, int* pRC)
{
	int Ligne;
	int Colonne;

	/* -----------------------------------------------------------------------
		   Étape 1 : Vérification du pointeur valide.
	   ----------------------------------------------------------------------- */
	if (pMatrice != 0)
	{
		/* -----------------------------------------------------------------------
		   Étape 2 : Vérification que les coordonnées sont dans les bornes.
		   ----------------------------------------------------------------------- */
		if (stAction.x1 < 0 || stAction.x1 >= TAILLE1 ||
			stAction.y1 < 0 || stAction.y1 >= TAILLE2 ||
			stAction.x2 < 0 || stAction.x2 >= TAILLE1 ||
			stAction.y2 < 0 || stAction.y2 >= TAILLE2)
		{
			*pRC = SUPPRESSION_ERR;
		}
		else
		{
			/* -----------------------------------------------------------------------
			   Étape 3 : Vérification de l'alignement horizontal.
			   Les 3 pions doivent être sur la même ligne (x1 == x2)
			   et sur 3 colonnes consécutives (y2 == y1 + 2).
			   ----------------------------------------------------------------------- */
			if (stAction.x1 == stAction.x2 && stAction.y2 == stAction.y1 + 2)
			{
				/* -----------------------------------------------------------------------
				   Étape 4 : Si une des 3 cases supprimées contient de la gélatine,
				   on la retire en mettant AvecGelatine à 0.
				   La gélatine disparait avec le pion qui se trouvait dessus.
				   ----------------------------------------------------------------------- */
				for (Colonne = stAction.y1; Colonne <= stAction.y2; Colonne++)
				{
					pMatrice->tMatrice[stAction.x1][Colonne].AvecGelatine = 0;
				}

				/* -----------------------------------------------------------------------
				   Étape 5 : Descente des pions du dessus d'une ligne.
				   On parcourt de bas en haut en partant de la ligne supprimée (x1)
				   jusqu'à la ligne 1. Pour chaque ligne, les 3 colonnes concernées
				   (y1, y1+1, y1+2) prennent la couleur de la case située une ligne
				   au-dessus d'elles — ce qui simule la chute des pions vers le bas.
				   ----------------------------------------------------------------------- */
				for (Ligne = stAction.x1; Ligne >= 1; Ligne--)
				{
					pMatrice->tMatrice[Ligne][stAction.y1].enCouleur =
						pMatrice->tMatrice[Ligne - 1][stAction.y1].enCouleur;
					pMatrice->tMatrice[Ligne][stAction.y1 + 1].enCouleur =
						pMatrice->tMatrice[Ligne - 1][stAction.y1 + 1].enCouleur;
					pMatrice->tMatrice[Ligne][stAction.y1 + 2].enCouleur =
						pMatrice->tMatrice[Ligne - 1][stAction.y1 + 2].enCouleur;
				}

				/* -----------------------------------------------------------------------
				   Étape 6 : La case libérée en haut de chaque colonne concernée (ligne 0)
				   reçoit une nouvelle couleur tirée aléatoirement parmi
				   les 5 couleurs disponibles.
				   ----------------------------------------------------------------------- */
				for (Colonne = stAction.y1; Colonne <= stAction.y2; Colonne++)
				{
					pMatrice->tMatrice[0][Colonne].enCouleur = (etCouleursCase)(rand() % 5);
				}

				*pRC = SUPPRESSION_OK;
			}
			else
			{
				/* Pions non alignés horizontalement : suppression refusée */
				*pRC = SUPPRESSION_ERR;
			}
		}
	}
	else
	{
		/* Pointeur invalide : on signale l'erreur sans toucher à la mémoire */
		*pRC = SUPPRESSION_ERR;
	}
}

/* =============================================================================================================
   SuppressionV4
   -------------------------------------------------------------------------------------------------------------
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

   Méthode :
	- Étape 1 : vérifier que le pointeur est valide
	- Étape 2 : vérifier que le numéro de colonne est dans les bornes
	- Étape 3 : supprimer la gélatine et remplacer toutes les cases par de nouveaux pions aléatoires
   ============================================================================================================ */
void SuppressionV4(etMatrice* pMatrice, etAction stAction, int* pRC)
{
	int Ligne;

	if (pMatrice != 0)
	{
		if (stAction.y1 < 0 || stAction.y1 >= TAILLE2)
		{
			*pRC = SUPPRESSION_ERR;
		}
		else
		{

			/* -----------------------------------------------------------------------
			   Étape 3 : Suppression de la gélatine et remplacement par de nouveaux pions.
			   On parcourt chaque case de la colonne et simultanément :
				- On supprime la gélatine (AvecGelatine = 0)
				- On assigne une nouvelle couleur aléatoire parmi les 5 couleurs disponibles.
			   Pas de gélatine sur les nouveaux pions.
			   ----------------------------------------------------------------------- */
			for (Ligne = 0; Ligne < TAILLE1; Ligne++)
			{
				pMatrice->tMatrice[Ligne][stAction.y1].AvecGelatine = 0;
				pMatrice->tMatrice[Ligne][stAction.y1].enCouleur = (etCouleursCase)(rand() % 5);
			}

			*pRC = SUPPRESSION_OK;
		}
	}
	else
	{
		/* Pointeur invalide : on signale l'erreur sans toucher à la mémoire */
		*pRC = SUPPRESSION_ERR;
	}
}

/* =============================================================================================================
   SuppressionH4
   -------------------------------------------------------------------------------------------------------------
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

   Méthode :
	- Étape 1 : vérifier que le pointeur est valide
	- Étape 2 : vérifier que le numéro de ligne est dans les bornes
	- Étape 3 : supprimer la gélatine et faire descendre les pions du dessus d'une ligne
	- Étape 4 : faire entrer 10 nouveaux pions aléatoires par la première ligne
   ============================================================================================================ */
void SuppressionH4(etMatrice* pMatrice, etAction stAction, int* pRC)
{
	int Ligne;
	int Colonne;

	if (pMatrice != 0)
	{
		if (stAction.x1 < 0 || stAction.x1 >= TAILLE1)
		{
			*pRC = SUPPRESSION_ERR;
		}
		else
		{
			/* -----------------------------------------------------------------------
				Étape 3 : Suppression de la gélatine et descente des pions du dessus.
				On parcourt chaque colonne. Pour chaque colonne :
				 - On supprime la gélatine sur la ligne concernée (AvecGelatine = 0)
				 - On parcourt de bas en haut en partant de la ligne supprimée (x1)
				   jusqu'à la ligne 1. Chaque case prend la couleur de la case située
				   une ligne au-dessus. La gélatine ne bouge pas — seule la couleur descend.
			   ----------------------------------------------------------------------- */
			for (Colonne = 0; Colonne < TAILLE2; Colonne++)
			{
				pMatrice->tMatrice[stAction.x1][Colonne].AvecGelatine = 0;

				for (Ligne = stAction.x1; Ligne >= 1; Ligne--)
				{
					pMatrice->tMatrice[Ligne][Colonne].enCouleur =
						pMatrice->tMatrice[Ligne - 1][Colonne].enCouleur;
				}
			}

			/* -----------------------------------------------------------------------
				Étape 4 : Les 10 cases de la première ligne reçoivent chacune une
				nouvelle couleur tirée aléatoirement parmi les 5 couleurs disponibles.
			   ----------------------------------------------------------------------- */
			for (Colonne = 0; Colonne < TAILLE2; Colonne++)
			{
				pMatrice->tMatrice[0][Colonne].enCouleur = (etCouleursCase)(rand() % 5);
			}

			*pRC = SUPPRESSION_OK;
		}
	}
	else
	{
		/* Pointeur invalide : on signale l'erreur sans toucher à la mémoire */
		*pRC = SUPPRESSION_ERR;
	}
}

/* =============================================================================================================
   Verification
   -------------------------------------------------------------------------------------------------------------
   But :
	- Vérifier que suite à la permutation de deux pions adjacents, il y a
	  bien un alignement de 3 ou 4 pions de même couleur en horizontal ou vertical.

   Paramètres :
	- pMatrice : adresse de la matrice à analyser (après permutation)
	- x1, y1   : coordonnées du premier pion permuté
	- x2, y2   : coordonnées du second pion permuté
	- pRC      : adresse du code de retour
				 VERIFICATION_OUI (1) : un alignement a été détecté
				 VERIFICATION_NON (0) : aucun alignement détecté ou pointeur invalide

   Méthode :
	- Étape 1 : vérifier que le pointeur est valide
	- Étape 2 : vérifier s'il y a un alignement horizontal ou vertical
				autour du premier pion permuté (x1, y1)
	- Étape 3 : vérifier s'il y a un alignement horizontal ou vertical
				autour du second pion permuté (x2, y2)
	- Étape 4 : retourner le résultat
   ============================================================================================================ */
void Verification(etMatrice* pMatrice, int x1, int y1, int x2, int y2, int* pRC)
{
	int Colonne;
	int Ligne;
	int Trouve;

	if (pRC == NULL)
		return;

	if (pMatrice == NULL)
	{
		*pRC = VERIFICATION_NON;
		return;
	}

	/* -----------------------------------------------------------------------
	   Initialisation du flag de détection à 0 (aucun alignement trouvé).
	   Ce flag passera à 1 dès qu'un alignement de 3 ou 4 pions sera détecté.
	   On arrête la recherche dès qu'un alignement est trouvé (Trouve == 1).
	   ----------------------------------------------------------------------- */
	Trouve = 0;

	/* -----------------------------------------------------------------------
	   Étape 2a : Vérification horizontale autour du premier pion (x1, y1).
	   On parcourt toutes les positions de départ possibles sur la ligne x1.
	   Pour chaque position de départ, on vérifie si une série de 4 ou 3 pions
	   identiques consécutifs contient la case (x1, y1).
	   Une série contient (x1, y1) si : y1 >= Colonne ET y1 <= Colonne + N-1
	   où N est la longueur de la série (3 ou 4).
	   Le 4 est vérifié en priorité pour éviter de détecter un sous-ensemble
	   d'une série de 4 comme une série de 3.
	   ----------------------------------------------------------------------- */
	for (Colonne = 0; Colonne < TAILLE2 && Trouve == 0; Colonne++)
	{
		/* Série de 4 pions horizontaux contenant (x1, y1) :
		   - On vérifie qu'on ne dépasse pas les bornes (Colonne + 3 < TAILLE2)
		   - On vérifie que (x1, y1) fait partie de la série
		   - On vérifie que les 4 cases ont la même couleur */
		if (Colonne + 3 < TAILLE2 &&
			y1 >= Colonne && y1 <= Colonne + 3 &&
			pMatrice->tMatrice[x1][Colonne].enCouleur ==
			pMatrice->tMatrice[x1][Colonne + 1].enCouleur &&
			pMatrice->tMatrice[x1][Colonne].enCouleur ==
			pMatrice->tMatrice[x1][Colonne + 2].enCouleur &&
			pMatrice->tMatrice[x1][Colonne].enCouleur ==
			pMatrice->tMatrice[x1][Colonne + 3].enCouleur)
		{
			Trouve = 1;
		}
		/* Série de 3 pions horizontaux contenant (x1, y1) :
		   - On vérifie qu'on ne dépasse pas les bornes (Colonne + 2 < TAILLE2)
		   - On vérifie que (x1, y1) fait partie de la série
		   - On vérifie que les 3 cases ont la même couleur */
		else if (Colonne + 2 < TAILLE2 &&
				 y1 >= Colonne && y1 <= Colonne + 2 &&
				 pMatrice->tMatrice[x1][Colonne].enCouleur ==
				 pMatrice->tMatrice[x1][Colonne + 1].enCouleur &&
				 pMatrice->tMatrice[x1][Colonne].enCouleur ==
				 pMatrice->tMatrice[x1][Colonne + 2].enCouleur)
		{
			Trouve = 1;
		}
	}

	/* -----------------------------------------------------------------------
	   Étape 2b : Vérification verticale autour du premier pion (x1, y1).
	   On parcourt toutes les positions de départ possibles sur la colonne y1.
	   Pour chaque position de départ, on vérifie si une série de 4 ou 3 pions
	   identiques consécutifs contient la case (x1, y1).
	   Une série contient (x1, y1) si : x1 >= Ligne ET x1 <= Ligne + N-1
	   où N est la longueur de la série (3 ou 4).
	   ----------------------------------------------------------------------- */
	for (Ligne = 0; Ligne < TAILLE1 && Trouve == 0; Ligne++)
	{
		/* Série de 4 pions verticaux contenant (x1, y1) :
		   - On vérifie qu'on ne dépasse pas les bornes (Ligne + 3 < TAILLE1)
		   - On vérifie que (x1, y1) fait partie de la série
		   - On vérifie que les 4 cases ont la même couleur */
		if (Ligne + 3 < TAILLE1 &&
			x1 >= Ligne && x1 <= Ligne + 3 &&
			pMatrice->tMatrice[Ligne][y1].enCouleur ==
			pMatrice->tMatrice[Ligne + 1][y1].enCouleur &&
			pMatrice->tMatrice[Ligne][y1].enCouleur ==
			pMatrice->tMatrice[Ligne + 2][y1].enCouleur &&
			pMatrice->tMatrice[Ligne][y1].enCouleur ==
			pMatrice->tMatrice[Ligne + 3][y1].enCouleur)
		{
			Trouve = 1;
		}
		/* Série de 3 pions verticaux contenant (x1, y1) :
		   - On vérifie qu'on ne dépasse pas les bornes (Ligne + 2 < TAILLE1)
		   - On vérifie que (x1, y1) fait partie de la série
		   - On vérifie que les 3 cases ont la même couleur */
		else if (Ligne + 2 < TAILLE1 &&
				 x1 >= Ligne && x1 <= Ligne + 2 &&
				 pMatrice->tMatrice[Ligne][y1].enCouleur ==
				 pMatrice->tMatrice[Ligne + 1][y1].enCouleur &&
				 pMatrice->tMatrice[Ligne][y1].enCouleur ==
				 pMatrice->tMatrice[Ligne + 2][y1].enCouleur)
		{
			Trouve = 1;
		}
	}
	/* -----------------------------------------------------------------------
	   Étape 3a : Vérification horizontale autour du second pion (x2, y2).
	   Même logique qu'à l'étape 2a mais pour le second pion permuté.
	   On parcourt toutes les positions de départ possibles sur la ligne x2.
	   ----------------------------------------------------------------------- */
	for (Colonne = 0; Colonne < TAILLE2 && Trouve == 0; Colonne++)
	{
		/* Série de 4 pions horizontaux contenant (x2, y2) */
		if (Colonne + 3 < TAILLE2 &&
			y2 >= Colonne && y2 <= Colonne + 3 &&
			pMatrice->tMatrice[x2][Colonne].enCouleur ==
			pMatrice->tMatrice[x2][Colonne + 1].enCouleur &&
			pMatrice->tMatrice[x2][Colonne].enCouleur ==
			pMatrice->tMatrice[x2][Colonne + 2].enCouleur &&
			pMatrice->tMatrice[x2][Colonne].enCouleur ==
			pMatrice->tMatrice[x2][Colonne + 3].enCouleur)
		{
			Trouve = 1;
		}
		/* Série de 3 pions horizontaux contenant (x2, y2) */
		else if (Colonne + 2 < TAILLE2 &&
				 y2 >= Colonne && y2 <= Colonne + 2 &&
				 pMatrice->tMatrice[x2][Colonne].enCouleur ==
				 pMatrice->tMatrice[x2][Colonne + 1].enCouleur &&
				 pMatrice->tMatrice[x2][Colonne].enCouleur ==
				 pMatrice->tMatrice[x2][Colonne + 2].enCouleur)
		{
			Trouve = 1;
		}
	}
	/* -----------------------------------------------------------------------
	   Étape 3b : Vérification verticale autour du second pion (x2, y2).
	   Même logique qu'à l'étape 2b mais pour le second pion permuté.
	   On parcourt toutes les positions de départ possibles sur la colonne y2.
	   ----------------------------------------------------------------------- */
	for (Ligne = 0; Ligne < TAILLE1 && Trouve == 0; Ligne++)
	{
		/* Série de 4 pions verticaux contenant (x2, y2) */
		if (Ligne + 3 < TAILLE1 &&
			x2 >= Ligne && x2 <= Ligne + 3 &&
			pMatrice->tMatrice[Ligne][y2].enCouleur ==
			pMatrice->tMatrice[Ligne + 1][y2].enCouleur &&
			pMatrice->tMatrice[Ligne][y2].enCouleur ==
			pMatrice->tMatrice[Ligne + 2][y2].enCouleur &&
			pMatrice->tMatrice[Ligne][y2].enCouleur ==
			pMatrice->tMatrice[Ligne + 3][y2].enCouleur)
		{
			Trouve = 1;
		}
		/* Série de 3 pions verticaux contenant (x2, y2) */
		else if (Ligne + 2 < TAILLE1 &&
				 x2 >= Ligne && x2 <= Ligne + 2 &&
				 pMatrice->tMatrice[Ligne][y2].enCouleur ==
				 pMatrice->tMatrice[Ligne + 1][y2].enCouleur &&
				 pMatrice->tMatrice[Ligne][y2].enCouleur ==
				 pMatrice->tMatrice[Ligne + 2][y2].enCouleur)
		{
			Trouve = 1;
		}
	}

	/* -----------------------------------------------------------------------
	   Étape 4 : Retour du résultat.
	   Si Trouve vaut 1, au moins un alignement de 3 ou 4 pions a été détecté
	   autour d'un des deux pions permutés → VERIFICATION_OUI.
	   Si Trouve vaut 0, aucun alignement n'a été trouvé → VERIFICATION_NON.
	   ----------------------------------------------------------------------- */
	*pRC = (Trouve == 1) ? VERIFICATION_OUI : VERIFICATION_NON;
}


/* =============================================================================================================
   Calcul
   -------------------------------------------------------------------------------------------------------------
   But :
	- Parcourir toute la grille et détecter les séries de 3 ou 4 pions
	  de même couleur alignés horizontalement ou verticalement.
	- Pour chaque série détectée, créer une action et l'ajouter dans la Queue.

   Paramètres :
	- pMatrice : adresse de la matrice à analyser
	- pQueue   : adresse de la file dans laquelle ajouter les actions détectées
	- pRC      : adresse du code de retour
				 CALCUL_OK  (1) : analyse effectuée avec succès
				 CALCUL_ERR (0) : pointeur invalide

   Méthode :
	- Étape 1 : vérifier que les pointeurs sont valides
	- Étape 2 : parcourir chaque ligne pour détecter les séries horizontales
				- Si 4 pions identiques consécutifs → action "suppression-h4"
				- Si 3 pions identiques consécutifs → action "suppression-h3"
	- Étape 3 : parcourir chaque colonne pour détecter les séries verticales
				- Si 4 pions identiques consécutifs → action "suppression-v4"
				- Si 3 pions identiques consécutifs → action "suppression-v3"
   ============================================================================================================ */
void Calcul(etMatrice* pMatrice, etQueue* pQueue, int* pRC)
{
	int Ligne;
	int Colonne;
	etAction stAction;

	/* -----------------------------------------------------------------------
	   Étape 1 : Vérification des pointeurs
	   ----------------------------------------------------------------------- */
	if (pMatrice == NULL || pQueue == NULL || pRC == NULL)
	{
		if (pRC != NULL)
			*pRC = CALCUL_ERR;
		return;
	}
	/* -----------------------------------------------------------------------
	   Étape 2 : Détection des séries horizontales.
	   On parcourt chaque ligne et on compare les cases consécutives.
	   On vérifie d'abord le 4 (prioritaire) puis le 3.
	   ----------------------------------------------------------------------- */
	for (Ligne = 0; Ligne < TAILLE1; Ligne++)
	{
		for (Colonne = 0; Colonne < TAILLE2; Colonne++)
		{
			/* -----------------------------------------------------------------------
			   Vérification d'une série de 4 pions horizontaux.
			   On s'assure qu'on ne dépasse pas les bornes (Colonne + 3 < TAILLE2).
			   ----------------------------------------------------------------------- */
			if (Colonne + 3 < TAILLE2 &&
				pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
				pMatrice->tMatrice[Ligne][Colonne + 1].enCouleur &&
				pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
				pMatrice->tMatrice[Ligne][Colonne + 2].enCouleur &&
				pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
				pMatrice->tMatrice[Ligne][Colonne + 3].enCouleur)
			{
				/* Création de l'action suppression-h4 */
				strcpy_s(stAction.nomAction, "suppression-h4");
				stAction.x1 = Ligne;
				stAction.y1 = Colonne;
				stAction.x2 = Ligne;
				stAction.y2 = Colonne + 3;
				AddQueue(pQueue, stAction);
				/* On saute les 3 cases suivantes pour éviter les doublons */
				Colonne += 3;
			}
			/* -----------------------------------------------------------------------
			   Vérification d'une série de 3 pions horizontaux.
			   On s'assure qu'on ne dépasse pas les bornes (Colonne + 2 < TAILLE2).
			   ----------------------------------------------------------------------- */
			else if (Colonne + 2 < TAILLE2 &&
					 pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
					 pMatrice->tMatrice[Ligne][Colonne + 1].enCouleur &&
					 pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
					 pMatrice->tMatrice[Ligne][Colonne + 2].enCouleur)
			{
				/* Création de l'action suppression-h3 */
				strcpy_s(stAction.nomAction, "suppression-h3");
				stAction.x1 = Ligne;
				stAction.y1 = Colonne;
				stAction.x2 = Ligne;
				stAction.y2 = Colonne + 2;
				AddQueue(pQueue, stAction);

				/* On saute les 2 cases suivantes pour éviter les doublons */
				Colonne += 2;
			}
		}
	}

	/* -----------------------------------------------------------------------
		Étape 3 : Détection des séries verticales.
		On parcourt chaque colonne et on compare les cases consécutives.
		On vérifie d'abord le 4 (prioritaire) puis le 3.
		----------------------------------------------------------------------- */
	for (Colonne = 0; Colonne < TAILLE2; Colonne++)
	{
		for (Ligne = 0; Ligne < TAILLE1; Ligne++)
		{
			/* -----------------------------------------------------------------------
			   Vérification d'une série de 4 pions verticaux.
			   On s'assure qu'on ne dépasse pas les bornes (Ligne + 3 < TAILLE1).
			   ----------------------------------------------------------------------- */
			if (Ligne + 3 < TAILLE1 &&
				pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
				pMatrice->tMatrice[Ligne + 1][Colonne].enCouleur &&
				pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
				pMatrice->tMatrice[Ligne + 2][Colonne].enCouleur &&
				pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
				pMatrice->tMatrice[Ligne + 3][Colonne].enCouleur)
			{
				/* Création de l'action suppression-v4 */
				strcpy_s(stAction.nomAction, "suppression-v4");
				stAction.x1 = Ligne;
				stAction.y1 = Colonne;
				stAction.x2 = Ligne + 3;
				stAction.y2 = Colonne;
				AddQueue(pQueue, stAction);

				/* On saute les 3 lignes suivantes pour éviter les doublons */
				Ligne += 3;
			}
			/* -----------------------------------------------------------------------
			   Vérification d'une série de 3 pions verticaux.
			   On s'assure qu'on ne dépasse pas les bornes (Ligne + 2 < TAILLE1).
			   ----------------------------------------------------------------------- */
			else if (Ligne + 2 < TAILLE1 &&
					 pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
					 pMatrice->tMatrice[Ligne + 1][Colonne].enCouleur &&
					 pMatrice->tMatrice[Ligne][Colonne].enCouleur ==
					 pMatrice->tMatrice[Ligne + 2][Colonne].enCouleur)
			{
				/* Création de l'action suppression-v3 */
				strcpy_s(stAction.nomAction, "suppression-v3");
				stAction.x1 = Ligne;
				stAction.y1 = Colonne;
				stAction.x2 = Ligne + 2;
				stAction.y2 = Colonne;
				AddQueue(pQueue, stAction);
				
				/* On saute les 2 lignes suivantes pour éviter les doublons */
				Ligne += 2;
			}
		}
	}

	/* -----------------------------------------------------------------------
	   Étape finale : succès
	   ----------------------------------------------------------------------- */
	*pRC = CALCUL_OK;
}

#endif