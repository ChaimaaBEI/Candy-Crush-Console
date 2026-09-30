#pragma once
#include "Action.h"

/* =========================================================================
   Constantes de configuration
   ========================================================================= */

   /* Capacité maximale du tableau qui stocke les actions.
	  (Version tableau fixe demandée dans l'exercice) */
#define TAILLE 200

	  /* Codes de retour pour AddQueue */
#define INSERTION_OK 1
#define FILE_PLEINE  0

/* Codes de retour pour GetQueue */
#define GET_OK       1
#define FILE_VIDE    0

/* =========================================================================
   Structure etQueue : file FIFO circulaire d'actions
   -------------------------------------------------------------------------
   - action  : tableau des actions en attente
   - top     : index du premier élément à lire (tête de file)
   - next    : index de la prochaine case libre (fin logique de file)
   - size    : capacité logique de la file (ici = TAILLE)

   Représentation importante :
   - File vide   <=> top == next
   - File pleine <=> (next + 1) % size == top
   ========================================================================= */
struct etQueue
{
	etAction action[TAILLE];
	int top;
	int next;
	int size;
};

/* =========================================================================
   Prototypes des fonctions de gestion de la file
   ========================================================================= */

   /* InitialiserQueue
	  -------------------------------------------------------------------------
	  But :
	  - Initialiser la structure etQueue avant utilisation.
	  Effet :
	  - top = 0, next = 0, size = TAILLE.
   */
void InitialiserQueue(etQueue* queue);

/* AddQueue
   -------------------------------------------------------------------------
   But :
   - Ajouter une action à la fin de la file.
   Paramètres :
   - queue  : adresse de la file à modifier
   - action : action à insérer
   Retour :
   - INSERTION_OK si insertion réussie
   - FILE_PLEINE  si la file est pleine
*/
int AddQueue(etQueue* queue, etAction action);

/* GetQueue
   -------------------------------------------------------------------------
   But :
   - Récupérer (et retirer) l'action en tête de file.
   Paramètres :
   - queue  : adresse de la file à lire/modifier
   - action : adresse de la variable de sortie qui recevra l'action
   Retour :
   - GET_OK    si une action a été récupérée
   - FILE_VIDE si la file est vide
*/
int GetQueue(etQueue* queue, etAction* action);