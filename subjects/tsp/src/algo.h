//
// Created by solarliner on 04/01/2020.
//

#ifndef L2_INFO_S3_ALGO_H
#define L2_INFO_S3_ALGO_H

#ifdef WITH_CAIRO
#include "draw.h"
#endif
#include "instance.h"
#include "utils.h"

/**
 * Calcule le chemin le plus court en utilisant une technique de brute-force
 * @param self Instance du TSP à optimiser
 * @param best Tournée à distance optimisée
 * @param worst Tournée la plus longue
 */
void instance_tour_bruteforce(instance_t *self, tour_t *best, tour_t *worst, const bool use_mat);
/**
 * Initialise un nouveau tour PPV basé sur l'instance actuelle
 * @param self Instance qui servira de base
 * @param tour Tour à initialiser
 * @param start Indice de ville de départ
 */
void instance_tour_ppv(instance_t *self, tour_t *tour);
/**
 * @brief Calcule un chemin par marche aléatoire
 * @param self Instance de base
 * @param result Tour résultat
 */
void instance_tour_random_walk(instance_t *self, tour_t *result);
/**
 * Effectue un swap 2-optimisation sur la tournée
 * @param self Instance du TSP
 * @param tour Tournée à modifier
 * @param i1 Premier indice à permuter
 * @param i2 Deuxième indice à permuter
 */
void instance_tour_swap_2opt(const instance_t *self, tour_t *tour, unsigned int i1, unsigned int i2);
bool instance_improve_2opt(instance_t *self, tour_t *tour);
/**
 * Effectue une 2-optimisation systématique sur tous les noeuds
 * @param self Instance du TSP
 * @param tour Tour à optimiser
 */
void instance_tour_2opt(instance_t *self, tour_t *tour, const bool use_ppv);
#ifdef REPORT
void instance_tour_genetic(instance_t *self, tour_t *tour, const int fi);
#else
void instance_tour_genetic(instance_t *self, tour_t *tour);
#endif

#endif // L2_INFO_S3_ALGO_H
