#ifndef _H_INSTANCE
#define _H_INSTANCE

#include "consts.h"
#include "matrice.h"
#include "node.h"
#include "tour.h"
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// SD pour lire le fichier d'instance TSPLIB
typedef struct instance_s {
	char name[TOUR_NAME_MAX_SIZE]; // nom de l'instance
	char type[TOUR_NAME_MAX_SIZE]; // type de l'instance
	char edge_type[TOUR_NAME_MAX_SIZE];
	double total_distance; // longueur de la tournée (calcul)
	mati_t visited_nodes;
	matdsym_t distances;
	vecnode_t nodes; // tableau des villes formant la tournée (dans l'ordre)
					 // (calcul)
} instance_t;

/**
 * Créée une instance du TSP
 * @param self instance non-initialisée
 */
void instance_init(instance_t *self);
/**
 * @brief Retourne la dimension de l'instance actuelle
 *
 * @param self instance
 * @return Dimension de l'instance actuelle
 */
unsigned int instance_dimension(const instance_t *self);
/**
 * Affiche le contenu de l'instance dans la sortie standard
 * @param self Instance à afficher
 */
void instance_dump(instance_t *self);
/**
 * Affiche la distance matricielle entre deux nœuds
 * @param self Instance à afficher
 * @param n1 Premier noeud
 * @param n2 Deuxième noeud
 */
void instance_dump_legdist(instance_t *self, unsigned int n1, unsigned int n2);
/**
 * Affiche la matrice de distances de l'instance
 * @param self Instance à afficher
 */
void instance_dump_matdist(const instance_t *self);
/**
 * Calcule la distance euclidienne entre deux noeuds de l'instance
 * @param self Instance en entrée
 * @param v1 Premier noeud
 * @param v2 Deuxième noeud
 * @returns Distance euclidienne entre les deux points
 */
double instance_dist_leg(instance_t *self, unsigned int v1, unsigned int v2);
/**
 * Retourne la distance sur un chemin de plusieurs noeuds
 * @param self sur laquelle calculer la distance
 * @param indices Tableau d'indices
 * @param n Nombre d'indices dans le tableau
 * @returns Distance euclidienne du tour
 */
double instance_dist_mat(instance_t *self, unsigned int *indices, int n);
/**
 * Retourne la distance sur un tour
 * @param self Instance sur laquelle calculer la distance
 * @param tour Tableau d'indices du tour
 * @return Distance euclidienne sur le tour complet
 */
double instance_tour_calc_length(const instance_t *self, tour_t *tour, const bool use_mat);
/**
 * Initialise l'instance avec le contenu du fichier en entrée
 * @param self Instance à modifier
 * @param fichier Fichier à lire
 */
void instance_read_file(instance_t *self, FILE *file);
/**
 * Calcule la distance du tour dans l'instance
 * @param self Instance à calculer
 * @returns Distance euclidienne du tour
 */
void instance_dist(instance_t *self);
/**
 * Calcule la distance du tour inclut dans l'instance et met à jour la distance
 * dans l'instance
 * @param self Instance à vérifier
 */
void instance_verify_dist(instance_t *self);
/**
 * @brief écrit l'instance dans le fichier, sous forme de CSV.
 * @param self Instance à exporter
 * @param file Fichier ouvert en écriture
 */
void instance_dump_csv(FILE *file, instance_t *self);
void instance_build_distmat(instance_t *self);
/**
 * @brief Libère la mémoire associée aà l'instance. Ne libère pas l'objet
 * lui-même.
 *
 * @param self Instance
 */
void instance_free(instance_t *self);

#endif
