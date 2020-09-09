#ifndef _H_TOUR
#define _H_TOUR

#include "consts.h"
#include "matrice.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// SD pour lire le fichier "tour" de TSPLIB
typedef struct tour_s {
	char name[TOUR_NAME_MAX_SIZE];	 // nom de l'instance
	char method[TOUR_NAME_MAX_SIZE]; // Nom de la méthode utilisée
	unsigned long ticks;			 // Nombre de tics d'horloge
	double total_distance;			 // longueur du tour (calculée ou lue)
	veci_t tour;					 // liste des noeuds de la tournée lus dans le fichier tour
} tour_t;

/**
 * Initialise un tour
 * @param tour Tour à initialiser
 */
void tour_init(tour_t *tour);
/**
 * @brief Initialise un tour avec une capacité prédéfinie
 *
 * @param tour Tour à initialiser
 * @param capacity Capacité du vecteur d'indices des tours
 */
void tour_init_capacity(tour_t *tour, unsigned int capacity);
/**
 * Affiche sur la sortie standard le contenu du tour.
 * @param tour Tour à afficher
 */
void tour_from_array(tour_t *tour, int *idx, unsigned int n);
/**
 * @brief Copie les éléments du tableau dans le tour, remplaçant le tour
 * précédent
 * @param tour Tour
 * @param idx Tableaux d'indices
 * @param n Nombre d'indices dans le tableau
 */
void tour_set_array(tour_t *tour, int *idx, unsigned int n);
/**
 * @brief Affiche le tour sur la sortie standard.
 * @param self Tour
 */
void tour_dump(const tour_t *self);
/**
 * @brief Retourne la dimension du tour actuel
 *
 * @param self Tour
 * @return Dimension du tour
 */
unsigned int tour_dimension(const tour_t *self);
/**
 * Copie le tour source dans le tour de destination, en dupliquant les tableaux
 * @param dest Tour de destination
 * @param src Tour de source
 */
void tour_hardcopy(tour_t *dest, const tour_t *src);
/**
 * Vérifie que le tour actuel aie le noeud passé en paramètre
 * @param tour Tour à regarder
 * @param node Noeud à vérifier
 * @returns Booléen, 1 si le noeud est inclus, 0 sinon.
 */
bool tour_has_node(tour_t *tour, int node);
/**
 * Ajoute le noeud au tour
 * @param tour Tour auquel ajouter le noeud
 * @param node Noeud à ajouter
 */
void tour_add(tour_t *tour, int node);
/**
 * Vérifie si les villes sont adjacentes
 * @param tour Tour sur lequel vérifier
 * @param n1 Premier noeud
 * @param n2 Deuxième noeud
 * @return 1 si les villes sont adjacents, 0 sinon
 */
bool tour_nodes_adjacent(tour_t *tour, int n1, int n2);
void tour_sort(tour_t *arr, unsigned int n);
void tour_dump_csv(FILE *file, tour_t *self);

void tour_free(tour_t *self);

#endif
