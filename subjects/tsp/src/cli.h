#ifndef _H_CLI
#define _H_CLI
#include "consts.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef WITH_CAIRO
#define NB_BALISES 13 // nombre max de balises de la ligne de commande
// liste des codes pour les balises
#else
#define NB_BALISES 12
#endif
#define BAL_F 0		// -f
#define BAL_T 1		// -t
#define BAL_V 2		// -v
#define BAL_O 3		// -o
#define BAL_PPV 4	//-ppv
#define BAL_BF 5	// -bf
#define BAL_BFM 6	// -bfm
#define BAL_RW 7	// -rw
#define BAL_2OPT 8	// -2opt
#define BAL_GA 9	// -ga
#define BAL_H 10	//-h
#define BAL_ZERO 11 // -nz no zéro
#ifdef WITH_CAIRO
#define BAL_DRAW 12
#endif

/**
 * Analyse les options de ligne de commande, et renvoie un tableau des balises
 * passées au programme.
 * @param argc Nombre d'arguements passés au programme
 * @param argv Tableau de chaînes de caractères passés au programme
 * @param tab_arg Tableau de sortie contenant les balises passées au programme
 */
void get_args(int argc, char **argv, bool tab_arg[NB_BALISES]);
/**
 * Affiche la documentation rapide en sortie standard.
 * @param argv0 Premier arguement, c'est à dire le nom du programme.
 */
void print_help(char *argv0);

#endif
