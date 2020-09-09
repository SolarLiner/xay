#ifndef _H_UTILS
#define _H_UTILS

#include "consts.h"
#include "matrice.h"
#include "node.h"
#include "tour.h"
#include <errno.h>
#include <math.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define SIGN(x) ((x) > 0 ? 1 : (x) < 0 ? -1 : 0)
#define FRACT(x) ((x)-floor(x))
#define RANDOM_DOUBLE() ((double)rand() / RAND_MAX)
#define RANDOM_RANGE(min, max) (rand() % ((max) - (min)) + (min))
#define RANDOM_CHOICE(tab, n) (tab)[RANDOM_RANGE(0, n)]
#define RANDOM_EITHER(a, b) (RANDOM_DOUBLE() > 0.5 ? (a) : (b))

// Forward declaration pour éviter un import circulaire
typedef struct node_s node_t;

/**
 * Affiche le message la sortie d'erreurs et quitte le programme
 * @param message Message à afficher
 */
void error(char *message);
/**
 * Vérifie sir le motif apparaît au débur du buffer.
 * @param motif Motif à vérifier
 * @param buf Buffer contenant possiblement le motif.
 * @return 1 si le motif est présent en début du buffer, 0 sinon.
 */
int prefix(char *motif, char *buf);
/**
 * @brief Copie les éléments de src dans dest
 *
 * @param dest Tableau de destination
 * @param src Tableau source
 * @param n Nombre d'éléments à copier
 */
void copy_array_int(int *dest, int *src, unsigned int n);
/**
 * @brief Affiche le tableau d'entier sur la sortie standard
 */
void print_array_int(int *tab, unsigned int n);
/**
 * @brief Écrit le contenu du tableau d'entiers dans le fichier file.
 *
 * @param file Fichier ouvert en écriture
 * @param tab Tableau à écrire
 * @param n Longueur du tableau
 */
void fprint_array_int(FILE *file, int *tab, unsigned int n);
/**
 * @brief Affiche un tableau de nœuds sur la sortie standard.
 *
 * @param tab Tableau à afficher
 * @param n Nombre d'éléments dans le tableau
 */
void print_array_node(node_t *tab, unsigned int n);
/**
 * @brief Écrit le contenu du tableau de nœuds dans le fichier file.
 *
 * @param file Fichier ouvert en écriture
 * @param tab Tableau à écrire
 * @param n Nombre d'éléments dans le tableau.
 */
void fprint_array_node(FILE *file, node_t *tab, unsigned int n);
/**
 * @brief Lit une ligne du fichier d'entrée.
 *
 * @param file Fichier à lire
 * @param n Taille maximale de la chaîne en sortie
 * @param output Chaîne de sortie
 * @return true si la lecture s'est poursuivie jusqu'à la fin du fichier
 * @return false si il reste des données à lire
 */
bool read_line(FILE *file, int n, char *output);
/**
 * @brief Affiche un tableau de doubles sur la sortie standard.
 * @param arr Tableau à afficher
 * @param n Nombre d'indices du tableau
 */
void print_array_double(const double *arr, unsigned int n);
/**
 * @brief Écrit le tableau de doubles dans le fichier spécifié.
 * @param file Fichier ouvert en écriture
 * @param arr Tableau à afficher
 * @param n Nombre d'indices du tableau
 */
void fprint_array_double(FILE *file, const double *arr, unsigned int n);
/**
 * @brief Vérifie que deux chaînes de caractères sont égales
 *
 * @param a Chaîne a
 * @param b Chaîne b
 * @return true si les chaînes sont égales
 */
bool streq(char *a, char *b);
/**
 * @brief Vérifie que deux flottants sont égaux à EPSILON près.
 *
 * @param a Flottant a
 * @param b Flottant b
 * @return true si les flottants sont égaux
 */
bool double_eq(double a, double b);

bool parse_param_int(unsigned int *dest, const char *input);

bool parse_param_double(double *dest, const char *input);

regex_t compile_regex(const char *input, int flags);

char *string_from_match(const char *input, regmatch_t *match);

unsigned int rand_prob(const double *p, const unsigned int n);

#endif
