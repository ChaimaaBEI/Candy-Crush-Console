#include <stdio.h>
#include "Affichage.h"

/* =========================================================================
   AfficherCase  (fonction interne)
   -------------------------------------------------------------------------
   But :
   - Afficher une seule case de la grille avec sa couleur de fond ANSI.
   - Si la case est recouverte de gélatine (AvecGelatine == 1),
     on affiche " G " en noir au centre, sinon "   " (vide).

   Paramètres :
   - pMatrice : adresse de la matrice
   - Ligne    : index de la ligne de la case
   - Colonne  : index de la colonne de la case

   Codes ANSI utilisés :
   - \033[103m : fond jaune clair
   - \033[42m  : fond vert
   - \033[104m : fond bleu clair
   - \033[101m : fond rouge clair
   - \033[105m : fond mauve
   - \033[30m  : texte noir (lisible sur tous les fonds)
   - \033[0m   : reset toutes les couleurs
   ========================================================================= */
static void AfficherCase(etMatrice* pMatrice, int Ligne, int Colonne)
{
    /* Contenu de la case : "G" centré si gélatine, sinon espaces */
    const char* contenu = (pMatrice->tMatrice[Ligne][Colonne].AvecGelatine == 1)
        ? " G " : "   ";

    /* Sélection de la couleur de fond selon enCouleur */
    if (pMatrice->tMatrice[Ligne][Colonne].enCouleur == Jaune)
        printf("\033[103m\033[30m%s\033[0m|", contenu);
    else if (pMatrice->tMatrice[Ligne][Colonne].enCouleur == Vert)
        printf("\033[42m\033[30m%s\033[0m|", contenu);
    else if (pMatrice->tMatrice[Ligne][Colonne].enCouleur == Bleu)
        printf("\033[104m\033[30m%s\033[0m|", contenu);
    else if (pMatrice->tMatrice[Ligne][Colonne].enCouleur == Rouge)
        printf("\033[101m\033[30m%s\033[0m|", contenu);
    else
        printf("\033[105m\033[30m%s\033[0m|", contenu); /* Mauve */
}

/* =========================================================================
   Affichage
   -------------------------------------------------------------------------
   But :
   - Afficher la matrice de jeu en console (ASCII + ANSI).
   - Chaque case est représentée par 1 seule ligne :
       couleur de fond + " G " si gélatine, sinon "   "
   - Largeur totale : 20 colonnes x 4 car. = 84 car. max
     → tient dans une console standard sans retour à la ligne forcé.
     (l'ancienne version utilisait 7 car. par case soit ~162 car.,
      ce qui provoquait des retours à la ligne intempestifs)

   Paramètre :
   - pMatrice : adresse de la matrice à afficher.
                Si NULL, la fonction ne fait rien (évite crash).
   ========================================================================= */
void Affichage(etMatrice* pMatrice)
{
    int Ligne;
    int Colonne;

    /* Si pointeur NULL, on ne fait rien */
    if (pMatrice == 0)
        return;

    /* -----------------------------------------------------------------------
       1) Titre encadré
       ----------------------------------------------------------------------- */
    printf("\n  +--------------------GRILLE DE JEU--------------------+\n\n");

    /* -----------------------------------------------------------------------
       2) Numéros de colonnes (repère visuel pour la saisie)
       ----------------------------------------------------------------------- */
    printf("     ");
    for (Colonne = 0; Colonne < TAILLE2; Colonne++)
        printf("%2d  ", Colonne);
    printf("\n");

    /* -----------------------------------------------------------------------
       3) Bordure supérieure de la grille
       ----------------------------------------------------------------------- */
    printf("    +");
    for (Colonne = 0; Colonne < TAILLE2; Colonne++)
        printf("---+");
    printf("\n");

    /* -----------------------------------------------------------------------
       4) Parcours des lignes de la matrice
       ----------------------------------------------------------------------- */
    for (Ligne = 0; Ligne < TAILLE1; Ligne++)
    {
        /* -------------------------------------------------------------------
           4.1) Numéro de ligne + cases colorées
                Chaque case : fond ANSI + " G " ou "   " + séparateur "|"
           ------------------------------------------------------------------- */
        printf("%3d |", Ligne);
        for (Colonne = 0; Colonne < TAILLE2; Colonne++)
            AfficherCase(pMatrice, Ligne, Colonne);
        printf("\n");

        /* -------------------------------------------------------------------
           4.2) Séparateur horizontal entre deux lignes de cases
           ------------------------------------------------------------------- */
        printf("    +");
        for (Colonne = 0; Colonne < TAILLE2; Colonne++)
            printf("---+");
        printf("\n");
    }

    /* -----------------------------------------------------------------------
       5) Légende
       -----------------------------------------------------------------------
       - Montre quelle couleur correspond à quel nom
       - Explique que "G" signifie gélatine
       ----------------------------------------------------------------------- */
    printf("\n  LEGENDE :\n\n");
    printf("  \033[103m\033[30m   \033[0m Jaune   ");
    printf("\033[42m\033[30m   \033[0m Vert    ");
    printf("\033[104m\033[30m   \033[0m Bleu    ");
    printf("\033[101m\033[30m   \033[0m Rouge   ");
    printf("\033[105m\033[30m   \033[0m Mauve\n\n");
    printf("  \033[103m\033[30m G \033[0m = case recouverte de gelatine\n\n");
}