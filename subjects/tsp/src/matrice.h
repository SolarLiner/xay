#ifndef _H_MATRICES
#define _H_MATRICES
#include "node.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Structures

typedef struct veci_s {
	unsigned int capacity;
	unsigned int len;
	int *data;
} veci_t;

typedef struct vecnode_s {
	unsigned int capacity;
	unsigned int len;
	node_t *data;
} vecnode_t;

typedef struct mati_s {
	int *data;
	unsigned int cols;
	unsigned int rows;
} mati_t;

typedef struct matd_s {
	double *data;
	unsigned int cols;
	unsigned int rows;
} matd_t;

typedef struct matdsym_s {
	double *data;
	unsigned int size;
} matdsym_t;

// ======== TABLEAUX =========

/**
 * Créée un nouveau tableau avec une dimension donnée. Ne pas oublier l'appel
 * à `tab_free` pour désallouer la mémoire. Cette fonction vérifie les
 * allocations, elle peut faire quitter le programme en cas de problème.
 * @param dim Taille du tableau
 * @return Tableau alloué.
 */
int *tab_init(int dim);
/**
 * Désalloue la mémoire associée au tableau.
 * @param tab Tableau à désallouer
 */
void tab_free(int *tab);
/**
 * Affiche le contenu du tableau sous forme d'entiers dans le fichier de
 * sortie passé en arguement.
 * @param tab Tableau à afficher
 * @param n Taille du tableau
 * @param fich Fichier, ouvert en écriture, dans lequel afficher le tableau
 * @return nombre d'octets écrit
 */
int tab_print_int(FILE *f, int *tab, int n);
/**
 * Affiche le contenu du tableau sous forme hexadécimale dans le fichier de
 * sortie passé en arguement.
 * @param tab Tableau à afficher
 * @param n Taille du tableau
 * @param fich Ficher, ouvert en écriture, dans lequel afficher le tableau
 * @return Nombre d'octets écrit
 */
int tab_print_char_hex(FILE *f, int *tab, int n);
/**
 * Affiche le contenu du tableau sous forme de caractères dans le fichier de
 * sortie passé en arguement.
 * @param tab Tableau à afficher
 * @param n Taille du tableau
 * @param fich Fichier, ouvert en écriture, dans lequel afficher le tableau
 * @return Nombre d'octets écrit
 */
int tab_print_char(FILE *f, char *tab, int n);
/**
 * Échange les valeurs dans le tableau, en place.
 * @param tab Tableau à modifier
 * @param i Indice du premier élément
 * @param j Indice du deuxième élément
 */
void tab_swap(int *tab, unsigned int i, unsigned int j);
/**
 * Renverse une partie du tableau entre `start` et `end`, en place.
 * @param tab Tableau à modifier
 * @param start Indice limite à gauche
 * @param end Indice limite à droite
 */
void tab_reverse(int *tab, int start, int end);
/**
 * Produit la permutation suivante, dans l'ordre lexicographique, en place.
 * @param tab Tableau d'entrée
 * @param n Taille du tableau
 */
bool tab_next_permutation(int *tab, unsigned int n);

/**
 * @brief Crée un nouveau vecteur avec la capacité spécifiée.
 * Une capacité de zéro ne produit aucune allocation.
 *
 * @param capacity Capacité initiale du vecteur.
 * @return veci_t* Vecteur alloué.
 */
veci_t *veci_with_capacity(unsigned int capacity);
/**
 * @brief Crée un nouveau vecteur vide. Aucune allocation n'est faite jusqu'à
 * l'insertion du premier élément.
 *
 * @return veci_t* Vecteur alloué.
 */
veci_t *veci_new();
/**
 * @brief Initialise un vecteur avec des valeurs par défaut.
 *
 * @param self vecteur à initialiser
 */
void veci_init(veci_t *self);
/**
 * @brief Réinitialise un vecteur d'entier
 * @param self Vecteur
 */
void veci_reinit(veci_t *self);
/**
 * @brief Vérifie si le vecteur est initialisé.
 * Le vecteur peut ne pas être initialisé si la capacité initiale donnée était
 * de 0. Dans ce cas aucune allocation n'a été faite et le tableau interne de
 * données est NULL.
 *
 * @param vec Vecteur
 * @return bool Si le vecteur à été initialisé
 */
bool veci_is_init(const veci_t *self);
/**
 * @brief Vérifie si le vecteur est vide. Cela veut dire qu'aucun élément n'est
 * présent dans le tableau, ou que le vecteur n'est pas initialisé (capacité
 * initiale de 0).
 *
 * @param vec Vecteur
 * @return bool Si le vecteur est vide
 */
bool veci_is_empty(const veci_t *self);
/**
 * @brief Retourne la valeur à la position x du vecteur.
 *
 * @param vec Vecteur
 * @param x Index de l'élément
 * @return int Valeur à la position x
 */
int veci_get(const veci_t *self, unsigned int x);
/**
 * @brief Modifie le vecteur, plaçant la valeur à la position x.
 *
 * @param vec Vecteur
 * @param x Index de position
 * @param val Valeur à copier dans le tableau.
 */
void veci_set(veci_t *self, unsigned int x, int val);
/**
 * @brief Redimensionne la longueur vecteur (et pas sa capacité)
 *
 * @param vec Vecteur
 * @param new_capacity Nouvelle longueur du tableau
 */
void veci_resize(veci_t *self, unsigned int new_capacity);
/**
 * @brief Ajoute une valeur à la fin du tableau
 *
 * @param self Vecteur
 * @param val Valeur à ajouter
 */
void veci_push(veci_t *self, int val);
/**
 * @brief Vérifie si le vecteur contient la valeur.
 *
 * @param self Vecteur
 * @param val Valeur à comparer
 * @return true si la valeur est dans le vecteur
 */
bool veci_includes(const veci_t *self, int val);
/**
 * @brief Enlève le dernier élément du tableau et le retourne.
 *
 * @param vec Vecteur
 * @return int Valeur retournée
 */
int veci_pop(veci_t *self);
/**
 * @brief Enlève la première occurence de l'élément
 * @param self Vecteur
 * @param val Valeur à enlever
 * @returns true si un élément a été enlevée
 */
bool veci_remove(veci_t *self, int val);
/**
 * @brief Enlève l'élément à l'indice spécifié
 * @param self Vecteur
 * @param idx Indice de l'élément à enlever
 */
void veci_remove_at(veci_t *self, unsigned int idx);
/**
 * @brief Retourne une référence sur le premier élément
 * @param self vecteur
 * @return Itérateur sur les données
 */
int *veci_begin(const veci_t *self);
/**
 * @brief Retourne une référence sur le dernier élément
 * @param self Vecteur
 * @return
 */
int *veci_end(const veci_t *self);
/**
 * @brief Retourne le prochain élément de la séquence, ou NULL si la séquence
 * est terminée
 * @param self Vecteur
 * @param iter Référence sur l'élément actuel
 * @return Référence sur le prochain élément, ou NULL si arrivé à la fin.
 */
const int *veci_next(const veci_t *self, const int *iter);
/**
 * @brief Libère la mémoire du tableau
 *
 * @param vec Vecteur à libérer
 */
void veci_free(veci_t *self);

/**
 * @brief Crée un nouveau vecteur avec la capacité spécifiée.
 * Une capacité de zéro ne produit aucune allocation.
 *
 * @param capacity Capacité initiale du vecteur.
 * @return veci_t* Vecteur alloué.
 */
vecnode_t *vecnode_with_capacity(unsigned int capacity);
/**
 * @brief Crée un nouveau vecteur vide. Aucune allocation n'est faite jusqu'à
 * l'insertion du premier élément.
 *
 * @return vecnode_t* Vecteur alloué.
 */
vecnode_t *vecnode_new();
/**
 * @brief Initialise un vecteur avec des valeurs par défaut
 * @param self Vecteur à initialiser
 */
void vecnode_init(vecnode_t *self);
/**
 * @brief Vérifie si le vecteur est initialisé.
 * Le vecteur peut ne pas être initialisé si la capacité initiale donnée était
 * de 0. Dans ce cas aucune allocation n'a été faite et le tableau interne de
 * données est NULL.
 *
 * @param vec Vecteur
 * @return bool Si le vecteur à été initialisé
 */
bool vecnode_is_init(const vecnode_t *vec);
/**
 * @brief Vérifie si le vecteur est vide. Cela veut dire qu'aucun élément n'est
 * présent dans le tableau, ou que le vecteur n'est pas initialisé (capacité
 * initiale de 0).
 *
 * @param vec Vecteur
 * @return bool Si le vecteur est vide
 */
bool vecnode_is_empty(const vecnode_t *vec);
/**
 * @brief Retourne la valeur à la position x du vecteur.
 *
 * @param vec Vecteur
 * @param x Index de l'élément
 * @return int Valeur à la position x
 */
node_t *vecnode_get(const vecnode_t *vec, unsigned int x);
/**
 * @brief Modifie le vecteur, plaçant la valeur à la position x.
 *
 * @param vec Vecteur
 * @param x Index de position
 * @param val Valeur à copier dans le tableau.
 */
void vecnode_set(vecnode_t *vec, unsigned int x, node_t *val);
/**
 * @brief Redimensionne la longueur vecteur (et pas sa capacité)
 *
 * @param vec Vecteur
 * @param new_length Nouvelle longueur du tableau
 */
void vecnode_resize(vecnode_t *vec, unsigned int new_length);
/**
 * @brief Ajoute une valeur à la fin du tableau
 *
 * @param vec Vecteur
 * @param val Valeur à ajouter
 */
void vecnode_push(vecnode_t *vec, node_t *val);
/**
 * @brief Enlève le dernier élément du tableau et le retourne.
 *
 * @param vec Vecteur
 * @return int Valeur retournée
 */
node_t vecnode_pop(vecnode_t *vec);
/**
 * @brief Retourne une référence sur le permier élément du vecteur
 * @param self Vecteur
 * @return Référence sur le premier élément du vecteur.
 */
node_t *vecnode_begin(const vecnode_t *self);
/**
 * @brief Retourne une référence sur le dernier élément du vecteur
 * @param self Vecteur
 * @return Référence sur le dernier élément du vecteur
 */
node_t *vecnode_end(const vecnode_t *self);
/**
 * @brief Retourne une référence sur le prochain élément du vecteur
 * @param self Vecteur
 * @return Référence sur le prochain élément du vecteur
 */
const node_t *vecnode_next(const vecnode_t *self, const node_t *cur);
/**
 * @brief Libère la mémoire du tableau
 *
 * @param vec Vecteur à libérer
 */
void vecnode_free(vecnode_t *vec);

// ========= MATRICES ===========
/**
 * Initialise une nouvelle matrice entière 2D, de lignes et collones définies.
 * Ne pas oublier d'apeller `imat_free` pour désallouer la mémoire.
 * Cette fonction vérifie les allocations, elle peut faire quitter le
 * programme en cas de problème.
 * @param rows Nombre de lignes
 * @param cols Nombre de colonnes
 * @return Matrice entière aux dimensions donnés en paramètre
 */
mati_t *mati_new(unsigned int rows, unsigned int cols);
/**
 * Initialise une nouvelle matrice entière 2D, de lignes et collones définies.
 * Ne pas oublier d'apeller `imat_free` pour désallouer la mémoire.
 * Cette fonction vérifie les allocations, elle peut faire quitter le
 * programme en cas de problème.
 * @param self Matrice
 * @param rows Nombre de lignes
 * @param cols Nombre de colonnes
 * @return Matrice entière aux dimensions donnés en paramètre
 */
void mati_init(mati_t *self, unsigned int rows, unsigned int cols);
/**
 * @brief Retourne la valeur d'une matrice à la position [x,y]
 *
 * @param self Matrice
 * @param x Position verticale (colonne)
 * @param y Position horizontale (ligne)
 * @return int Valeur à la position [x,y]
 */
int mati_get(mati_t *self, unsigned int x, unsigned int y);
/**
 * @brief Modifie la matrice, mettant la valeur dans la case [x,y].
 *
 * @param self Matrice
 * @param x Position verticale (colonne)
 * @param y Position horizontale (ligne)
 * @param v Valeur à copier dans la matrice
 */
void mati_set(mati_t *self, unsigned int x, unsigned int y, int v);
/**
 * Libère la mémoire associée à la matrice entière passée en paramètre. ne
 * libère *pas* le pointeur vers la structure matrice.
 * @param m Matrice entière à désallouer
 */
void mati_free(mati_t *m);
/**
 * Initialise une nouvelle matrice double 2D, de lignes et collones définies.
 * Ne pas oublier d'apeller `dmat_free` pour désallouer la mémoire.
 * Cette fonction vérifie les allocations, elle peut faire quitter le
 * programme en cas de problème.
 * @param rows Nombre de lignes
 * @param cols Nombre de colonnes
 * @return Matrice double aux dimensions donnés en paramètre
 */
matd_t *matd_init(unsigned int rows, unsigned int cols);
/**
 * @brief Retourne la valeur dans la matrice stockée à la case [x,y]
 *
 * @param self Matrice
 * @param x Position verticale (colonne)
 * @param y Position horizontale (ligne)
 * @return double Valeur stockée à la case [x,y]
 */
double matd_get(matd_t *self, unsigned int x, unsigned int y);
/**
 * @brief Modifie la matrice, plaçant la valeur dans la case [x,y]
 *
 * @param self Matrice
 * @param x Position verticale (colonne)
 * @param y Position horizontale (ligne)
 */
void matd_set(matd_t *self, unsigned int x, unsigned int y, double v);
/**
 * Libère la mémoire associée à la matrice double passée en paramètre.
 * @param m Matrice double à désallouer
 */
void matd_free(matd_t *m);
/**
 * @brief Crée une nouvelle matrice symmétrique de la taille spécifiée
 * @param size
 * @return Matrice initialisée
 */
matdsym_t *matdsym_new(unsigned int size);
/**
 * @brief Initialise une nouvelle matrice symétrique.
 *
 * @param size Longueur d'un côté de la matrice
 * @return matdsym_t* Matrice allouée (à libérer après utilisation)
 */
void matdsym_init(matdsym_t *self, unsigned int size);
unsigned int matdsym_calculate_storage_size(matdsym_t *self);
/**
 * @brief Retourne la valeur à la case [x,y] et [y,x]
 *
 * @param self Matrice
 * @param x Position verticale du triangle supérieur (colonne)
 * @param y Position horizontale du triangle supérieur (ligne)
 * @return double Valeur à la case [x,y] et [y,x]
 */
double matdsym_get(const matdsym_t *self, unsigned int x, unsigned int y);
/**
 * @brief Modifie la matrice en place, mettant la valeur dans [x,y] et [y,x].
 *
 * @param self Matrice
 * @param x Position verticale du triangle supérieur (colonne)
 * @param y Position horizontale du triangle supérieur (ligne)
 * @param val Valeur à copier dans la matrice
 */
void matdsym_set(matdsym_t *self, unsigned int x, unsigned int y, double val);
/**
 * @brief Libère la mémoire associée à la matrice.
 *
 * @param self Matrice
 */
void matdsym_free(matdsym_t *self);

#endif
