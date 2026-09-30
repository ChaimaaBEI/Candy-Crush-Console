/* =============================================================================================================
   Candy.cpp  —  Point d'entrée du jeu Candy Crush Console
   -------------------------------------------------------------------------------------------------------------
   Principe général :
     Le jeu repose sur une file FIFO (etQueue) qui pilote l'enchaînement des actions.
     Au lieu d'appeler les fonctions directement, on empile des actions nommées dans
     la Queue, puis on les dépile une par une dans la boucle principale (dispatch).

     Séquence des actions (pseudo-code de l'énoncé) :
       INITIALISATION → AFFICHAGE → LECTURE → DEPLACEMENT
       → CALCUL → (SUPPRESSION-H ou SUPPRESSION-V) → CALCUL unique
       → VERIFICATION → AFFICHAGE → LECTURE → ...

     Fin de niveau :
       Quand VERIFICATION constate qu'il ne reste plus de gélatine,
       elle pousse FIN-NIVEAU qui termine la boucle du niveau courant.

   Fichiers requis :
     Matrice.h / Matrice.cpp     : grille, déplacements, suppressions, calcul
     Queue.h   / Queue.cpp       : file FIFO circulaire
     Affichage.h / Affichage.cpp : rendu ANSI coloré en console
   ============================================================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Matrice.h"
#include "Queue.h"
#include "Affichage.h"

   /* =============================================================================================================
      Noms des actions
      -------------------------------------------------------------------------------------------------------------
      Chaque constante correspond à une étape du jeu.
      Ces chaînes sont stockées dans etAction.nomAction (tableau de 20 caractères max,
      donc chaque nom fait au maximum 19 caractères + '\0').
      ============================================================================================================ */
#define ACT_INITIALISATION  "initialisation"   /* Initialise la matrice et la gélatine          */
#define ACT_AFFICHAGE       "affichage"         /* Affiche la grille à l'écran                   */
#define ACT_LECTURE         "lecture"           /* Lit les coordonnées des deux pions à permuter */
#define ACT_DEPLACEMENT     "deplacement"       /* Intervertit deux pions                        */
#define ACT_CALCUL          "calcul"            /* Détecte les alignements dans la grille        */
#define ACT_SUPPRESSION_H3  "suppression-h3"   /* Supprime 3 pions alignés horizontalement      */
#define ACT_SUPPRESSION_H4  "suppression-h4"   /* Supprime toute une ligne                      */
#define ACT_SUPPRESSION_V3  "suppression-v3"   /* Supprime 3 pions alignés verticalement        */
#define ACT_SUPPRESSION_V4  "suppression-v4"   /* Supprime toute une colonne                    */
#define ACT_VERIFICATION    "verification"      /* Vérifie s'il reste de la gélatine             */
#define ACT_FIN_NIVEAU      "fin-niveau"        /* Déclenché quand toute la gélatine est retirée */

      /* Nombre de cases recouvertes de gélatine au début de chaque niveau */
#define NBR_GELATINE 15

/* =============================================================================================================
   PushAction
   -------------------------------------------------------------------------------------------------------------
   But :
     Construire une etAction et l'insérer en fin de Queue.
     Centralise l'appel à AddQueue pour ne pas répéter la construction
     de la structure à chaque endroit du code.

   Comportement si la Queue est pleine :
     Affiche un message d'erreur et arrête immédiatement le programme
     (conformément à l'énoncé : la Queue pleine est une erreur fatale).

   Paramètres :
     pQueue         : adresse de la file
     nom            : nom de l'action à insérer (une des constantes ACT_...)
     x1, y1, x2, y2 : coordonnées associées à l'action (0,0,0,0 si inutilisées)
   ============================================================================================================ */
static void PushAction(etQueue* pQueue, const char* nom,
    int x1, int y1, int x2, int y2)
{
    etAction a;

    strcpy_s(a.nomAction, nom);
    a.x1 = x1;
    a.y1 = y1;
    a.x2 = x2;
    a.y2 = y2;

    if (AddQueue(pQueue, a) == FILE_PLEINE)
    {
        printf("\n[ERREUR FATALE] Queue pleine lors de l'ajout de \"%s\".\n"
            "Augmentez #define TAILLE dans Queue.h et recompilez.\n", nom);
        exit(1);
    }
}

/* =============================================================================================================
   ActionLecture
   -------------------------------------------------------------------------------------------------------------
   But :
     Demander au joueur les coordonnées des deux pions qu'il souhaite permuter,
     valider que chaque valeur est dans les bornes [0, TAILLE-1],
     puis pousser DEPLACEMENT avec ces coordonnées dans la Queue.

   Saisie :
     Quatre scanf_s distincts (un par valeur) pour éviter tout problème
     de parsing lorsque le joueur appuie sur Entrée entre les deux chiffres.

   Paramètre :
     pQueue : adresse de la file dans laquelle pousser DEPLACEMENT
   ============================================================================================================ */
static void ActionLecture(etQueue* pQueue)
{
    int x1, y1, x2, y2;
    int ok = 0;

    while (!ok)
    {
        printf("  Premier pion  - ligne   (0-%d) : ", TAILLE1 - 1);
        scanf_s("%d", &x1);
        printf("  Premier pion  - colonne (0-%d) : ", TAILLE2 - 1);
        scanf_s("%d", &y1);
        printf("  Deuxieme pion - ligne   (0-%d) : ", TAILLE1 - 1);
        scanf_s("%d", &x2);
        printf("  Deuxieme pion - colonne (0-%d) : ", TAILLE2 - 1);
        scanf_s("%d", &y2);

        if (x1 >= 0 && x1 < TAILLE1 && y1 >= 0 && y1 < TAILLE2 &&
            x2 >= 0 && x2 < TAILLE1 && y2 >= 0 && y2 < TAILLE2)
        {
            ok = 1;
        }
        else
        {
            printf("  Coordonnees invalides, recommencez.\n\n");
        }
    }

    PushAction(pQueue, ACT_DEPLACEMENT, x1, y1, x2, y2);
}

/* =============================================================================================================
   ActionVerification
   -------------------------------------------------------------------------------------------------------------
   But :
     Parcourir toute la grille pour déterminer s'il reste de la gélatine.
     - S'il en reste au moins une case → ne rien faire (le jeu continue).
     - Si la grille est entièrement dégelée → pousser FIN-NIVEAU.

   Paramètres :
     pMatrice : adresse de la grille à inspecter
     pQueue   : adresse de la file (pour pousser FIN-NIVEAU si besoin)
   ============================================================================================================ */
static void ActionVerification(etMatrice* pMatrice, etQueue* pQueue)
{
    int i, j;

    for (i = 0; i < TAILLE1; i++)
        for (j = 0; j < TAILLE2; j++)
            if (pMatrice->tMatrice[i][j].AvecGelatine)
                return;   /* Il reste de la gélatine : le niveau continue */

    /* Aucune gélatine → niveau terminé */
    PushAction(pQueue, ACT_FIN_NIVEAU, 0, 0, 0, 0);
}

/* =============================================================================================================
   TailleQueue
   -------------------------------------------------------------------------------------------------------------
   But :
     Calculer le nombre d'éléments présents dans la Queue en tenant compte
     du caractère circulaire de la file (next peut être < top après un wrap).

   Formule :
     (next - top + size) % size
     → résultat correct même quand next < top

   Retour :
     Nombre d'éléments présents (0 si vide)
   ============================================================================================================ */
static int TailleQueue(etQueue* pQueue)
{
    return (pQueue->next - pQueue->top + pQueue->size) % pQueue->size;
}

/* =============================================================================================================
   main
   -------------------------------------------------------------------------------------------------------------
   But :
     Orchestrer les 3 niveaux du jeu.
     Pour chaque niveau, initialiser la Queue avec INITIALISATION puis
     dispatcher les actions une par une jusqu'à FIN-NIVEAU ou Queue vide.

   Séquence complète (conforme au pseudo-code de l'énoncé) :
     INITIALISATION → AFFICHAGE → LECTURE → DEPLACEMENT
     → CALCUL → suppressions → CALCUL → VERIFICATION → AFFICHAGE → ...
   ============================================================================================================ */
int main(void)
{
    etMatrice stMatrice;
    etQueue   stQueue;
    etAction  stAction;
    int       Niveau;
    int       RC;
    int       NiveauTermine;
    int       tailleAvant;
    int       tailleApres;

    srand((unsigned int)time(NULL));

    printf("========================================\n");
    printf("         CANDY CRUSH  --  Console       \n");
    printf("========================================\n");

    /* ------------------------------------------------------------------
       Boucle sur les 3 niveaux (conforme à l'énoncé : Niveau <= 3)
       ------------------------------------------------------------------ */
    for (Niveau = 1; Niveau <= 3; Niveau++)
    {
        printf("\n========================================\n");
        printf("  NIVEAU %d\n", Niveau);
        printf("========================================\n");

        InitialiserQueue(&stQueue);
        NiveauTermine = 0;

        /* Premier élément imposé par l'énoncé :
           AddQueue(INITIALISATION) lance toute la chaîne */
        PushAction(&stQueue, ACT_INITIALISATION, 0, 0, 0, 0);

        /* ------------------------------------------------------------------
           Boucle principale de dispatch
           On traite les actions une par une tant que :
             - la Queue n'est pas vide (GetQueue retourne GET_OK)
             - le niveau n'est pas terminé (FIN-NIVEAU non traité)
           ------------------------------------------------------------------ */
        while (!NiveauTermine && GetQueue(&stQueue, &stAction) == GET_OK)
        {
            /* ============================================================
               ACTION : INITIALISATION
               ------------------------------------------------------------
               Remplit la grille avec des pions aléatoires et place
               NBR_GELATINE gélatines à des positions aléatoires.
               Enchaîne directement sur AFFICHAGE — on ne passe PAS par
               CALCUL ici pour ne pas saturer la Queue avec les séries
               déjà présentes dans la grille initiale.
               ============================================================ */
            if (strcmp(stAction.nomAction, ACT_INITIALISATION) == 0)
            {
                InitialisationMatrice(&stMatrice, NBR_GELATINE, &RC);
                PushAction(&stQueue, ACT_AFFICHAGE, 0, 0, 0, 0);
            }

            /* ============================================================
               ACTION : AFFICHAGE
               ------------------------------------------------------------
               Efface la console et affiche la grille ANSI colorée
               ainsi que la légende.
               Enchaîne sur LECTURE pour attendre la saisie du joueur.
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_AFFICHAGE) == 0)
            {
                system("cls");
                printf("\n  Niveau %d\n", Niveau);
                Affichage(&stMatrice);
                PushAction(&stQueue, ACT_LECTURE, 0, 0, 0, 0);
            }

            /* ============================================================
               ACTION : LECTURE
               ------------------------------------------------------------
               Demande au joueur les coordonnées des deux pions (4 saisies
               séparées : ligne1, col1, ligne2, col2).
               Valide que les coordonnées sont dans les bornes [0, TAILLE-1]
               puis pousse DEPLACEMENT avec ces coordonnées.
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_LECTURE) == 0)
            {
                ActionLecture(&stQueue);
            }

            /* ============================================================
               ACTION : DEPLACEMENT
               ------------------------------------------------------------
               Intervertit les deux pions dont les coordonnées sont
               stockées dans stAction (x1,y1) et (x2,y2).
               Conformément à l'énoncé, aucune vérification d'adjacence
               n'est effectuée : le joueur est libre de choisir n'importe
               quelles deux cases.
               Enchaîne sur CALCUL avec les coordonnées du déplacement.
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_DEPLACEMENT) == 0)
            {
                Deplacement(&stMatrice, stAction, &RC);

                /* Après déplacement → CALCUL pour détecter les alignements */
                PushAction(&stQueue, ACT_CALCUL,
                    stAction.x1, stAction.y1,
                    stAction.x2, stAction.y2);
            }

            /* ============================================================
               ACTION : CALCUL
               ------------------------------------------------------------
               Parcourt toute la grille à la recherche de séries de 3 ou 4
               pions identiques consécutifs (H et V).
               Calcul ajoute directement les suppressions dans la Queue.

               Détection des suppressions :
                 On mesure la taille de la Queue AVANT et APRÈS l'appel.
                 Si elle a augmenté → des suppressions ont été ajoutées :
                   on pousse UN SEUL CALCUL après elles pour traiter
                   les nouvelles séries créées par la chute des pions.
                 Si elle n'a pas augmenté → grille stable :
                   on passe à VERIFICATION puis AFFICHAGE.

               Note sur TailleQueue :
                 On utilise (next-top+size)%size et non next!=nextAvant
                 car la Queue est circulaire : next peut avoir wrappé à 0.
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_CALCUL) == 0)
            {
                tailleAvant = TailleQueue(&stQueue);

                Calcul(&stMatrice, &stQueue, &RC);

                if (RC == CALCUL_ERR)
                {
                    printf("[ERREUR] Calcul : pointeur invalide.\n");
                }
                else
                {
                    tailleApres = TailleQueue(&stQueue);

                    if (tailleApres > tailleAvant)
                    {
                        /* Des suppressions ont été ajoutées dans la Queue.
                           On pousse UN SEUL CALCUL après toutes les
                           suppressions pour traiter la chute des pions
                           une fois qu'elles seront toutes exécutées. */
                        PushAction(&stQueue, ACT_CALCUL, 0, 0, 0, 0);
                    }
                    else
                    {
                        /* Aucun alignement détecté : grille stable.
                           Conformément à l'énoncé, on ne gère pas
                           l'annulation de coup — on passe directement
                           à VERIFICATION puis AFFICHAGE. */
                        PushAction(&stQueue, ACT_VERIFICATION, 0, 0, 0, 0);
                        PushAction(&stQueue, ACT_AFFICHAGE, 0, 0, 0, 0);
                    }
                }
            }

            /* ============================================================
               ACTION : SUPPRESSION-H3
               ------------------------------------------------------------
               Supprime 3 pions alignés horizontalement.
               Les pions au-dessus descendent d'une ligne par colonne.
               3 nouveaux pions aléatoires entrent par le haut.
               La gélatine présente sur ces cases est retirée.
               Pas de CALCUL ici : le CALCUL unique est poussé par
               le bloc ACT_CALCUL après toutes les suppressions.
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_SUPPRESSION_H3) == 0)
            {
                SuppressionH3(&stMatrice, stAction, &RC);
            }

            /* ============================================================
               ACTION : SUPPRESSION-H4
               ------------------------------------------------------------
               Supprime toute la ligne stAction.x1.
               Les pions des lignes supérieures descendent d'une ligne.
               20 nouveaux pions aléatoires entrent par la ligne 0.
               La gélatine de la ligne est retirée.
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_SUPPRESSION_H4) == 0)
            {
                SuppressionH4(&stMatrice, stAction, &RC);
            }

            /* ============================================================
               ACTION : SUPPRESSION-V3
               ------------------------------------------------------------
               Supprime 3 pions alignés verticalement dans la colonne.
               Les pions au-dessus descendent de 3 cases.
               3 nouveaux pions aléatoires entrent par le haut.
               La gélatine présente sur ces cases est retirée.
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_SUPPRESSION_V3) == 0)
            {
                SuppressionV3(&stMatrice, stAction, &RC);
            }

            /* ============================================================
               ACTION : SUPPRESSION-V4
               ------------------------------------------------------------
               Supprime toute la colonne stAction.y1.
               20 nouveaux pions aléatoires remplacent la colonne entière.
               La gélatine de la colonne est retirée.
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_SUPPRESSION_V4) == 0)
            {
                SuppressionV4(&stMatrice, stAction, &RC);
            }

            /* ============================================================
               ACTION : VERIFICATION
               ------------------------------------------------------------
               Parcourt la grille case par case.
               Dès qu'une gélatine est trouvée → retour immédiat,
               le niveau n'est pas terminé, rien n'est poussé.
               Si aucune gélatine n'est trouvée → pousse FIN-NIVEAU.
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_VERIFICATION) == 0)
            {
                ActionVerification(&stMatrice, &stQueue);
            }

            /* ============================================================
               ACTION : FIN-NIVEAU
               ------------------------------------------------------------
               Affiche la grille finale et félicite le joueur.
               Met NiveauTermine à 1 pour sortir de la boucle while
               et passer au niveau suivant (ou terminer si Niveau == 3).
               ============================================================ */
            else if (strcmp(stAction.nomAction, ACT_FIN_NIVEAU) == 0)
            {
                system("cls");
                printf("\n  Niveau %d\n", Niveau);
                Affichage(&stMatrice);
                printf("\n  *** Bravo ! Toute la gelatine a ete eliminee ! ***\n");
                printf("  *** Niveau %d termine !                          ***\n\n",
                    Niveau);
                NiveauTermine = 1;
            }

        } /* fin while — Queue vide ou FIN-NIVEAU traité */

    } /* fin for — 3 niveaux joués */

    printf("\n========================================\n");
    printf("  Partie terminee. Merci d'avoir joue !\n");
    printf("========================================\n");

    return 0;
}