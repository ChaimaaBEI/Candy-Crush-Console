#include "Queue.h"
#include "Action.h"

/* =========================================================================
   InitialiserQueue
   -------------------------------------------------------------------------
   But :
   - Mettre la file dans un état vide avant toute utilisation.

   Principe :
   - top  : index du premier élément à lire
   - next : index de la prochaine case libre pour écrire
   - Une file vide est représentée par top == next

   Initialisation choisie :
   - top = 0
   - next = 0
   - size = TAILLE (capacité maximale du tableau)
   ========================================================================= */
void InitialiserQueue(etQueue* queue)
{
	queue->top = 0;
	queue->next = 0;
	queue->size = TAILLE;
}

/* =========================================================================
   AddQueue
   -------------------------------------------------------------------------
   But :
   - Ajouter une action à la fin de la file FIFO.

   Méthode :
   1) Calculer l'index "suivant" de next avec modulo (anneau circulaire)
   2) Détecter une file pleine :
	  - si suivant == top, il n'y a plus de case libre
   3) Si place disponible :
	  - copier l'action dans action[next]
	  - avancer next vers suivant

   Retours :
   - INSERTION_OK : insertion réalisée
   - FILE_PLEINE  : insertion impossible
   ========================================================================= */
int AddQueue(etQueue* queue, etAction action)
{
	/* Calcul de la prochaine case libre en mode circulaire.
	   Le modulo permet de revenir à 0 en fin de tableau. */
	int suivant = (queue->next + 1) % queue->size;

	/* Test de débordement :
	   si la prochaine case libre est top, la file est pleine. */
	if (suivant == queue->top)
		return FILE_PLEINE;

	/* Insertion de l'action à la position de fin actuelle. */
	queue->action[queue->next] = action;

	/* Mise à jour de la fin de file vers la prochaine case libre. */
	queue->next = suivant;

	return INSERTION_OK;
}

/* =========================================================================
   GetQueue
   -------------------------------------------------------------------------
   But :
   - Récupérer l'action la plus ancienne (FIFO) et la retirer de la file.

   Méthode :
   1) Vérifier si la file est vide :
	  - top == next  -> aucun élément à lire
   2) Sinon :
	  - copier l'élément de tête dans la variable de sortie (*action)
	  - avancer top d'une case (avec modulo)

   Retours :
   - GET_OK    : action récupérée avec succès
   - FILE_VIDE : aucune action disponible
   ========================================================================= */
int GetQueue(etQueue* queue, etAction* action)
{
	/* File vide : pas d'action à retourner. */
	if (queue->top == queue->next)
		return FILE_VIDE;

	/* Copie de l'action en tête vers la variable de sortie. */
	*action = queue->action[queue->top];

	/* Retrait logique de l'élément : on avance la tête de file. */
	queue->top = (queue->top + 1) % queue->size;

	return GET_OK;
}