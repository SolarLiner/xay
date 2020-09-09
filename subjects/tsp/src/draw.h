//
// Created by solarliner on 31/12/2019.
//

#ifndef L2_INFO_S3_DRAW_H
#define L2_INFO_S3_DRAW_H

#ifdef HAS_CAIRO

#include "instance.h"
#include <cairo/cairo.h>
#include <math.h>

/**
 * @brief Dessine les nœuds d'une instance
 * @param surface Surface de dessin de Cairo
 * @param instance Instance à dessiner
 */
void draw_instance(cairo_surface_t *surface, instance_t *instance);
/**
 * @brief Dessine un tour basé sur l'instance
 * @param surface Surface de dessin Cairo
 * @param instance Instance de base
 * @param tour Tour à dessiner
 */
void draw_tour(cairo_surface_t *surface, instance_t *instance, tour_t *tour);
/**
 * @brief Dessine une image de l'instance et du tour associé dans un fichier
 * PNG.
 * @param inst Instance
 * @param tour Tour
 * @param filename Nom du fichier de sortie.
 */
void draw_instance_tour(instance_t *inst, tour_t *tour, char *filename);

#endif
#endif // L2_INFO_S3_DRAW_H
