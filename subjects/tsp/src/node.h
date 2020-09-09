#ifndef _H_NODE
#define _H_NODE

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct node_s {
	double x;
	double y;
} node_t;

/**
 * @brief Affiche le nœud actuel sur la sortie standard.
 *
 * @param node
 */
void print_node(node_t *node);
/**
 * @brief Calcule la distance euclidienne entre les deux nœuds
 *
 * @param a Premier nœud
 * @param b Second nœud
 * @return double
 */
double node_distance(node_t *a, node_t *b);

bool node_eq(const node_t *a, const node_t *b);

void fprint_node(FILE *file, node_t *self);

bool line_intersect(const node_t *a1, const node_t *a2, const node_t *b1, const node_t *b2);

#endif
