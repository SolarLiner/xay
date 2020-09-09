#ifndef _H_CONSTS
#define _H_CONSTS
#include <stdbool.h>
#include <stdio.h>

#define TOUR_NAME_MAX_SIZE 32 // les noms sont limités à 32 caractères
#define MAXBUF 256			  // taille du buffer de lecture des fichiers
#define NIL -1				  // indique l'échec dans une fonction
#define EPSILON 1e-10		  // Différence flottante absolue considérée comme insignificante

typedef struct ga_params_s {
	unsigned int population;
	bool with_2opt;
	unsigned int epochs;
	double mutation;
} ga_params_t;

// variables externes
// permet d'éviter de se trainer les mêmes paramètres dans les sous-programmes
extern FILE *logfp;		 // fichier log
extern FILE *tourfp;	 // Fichier de tour
extern FILE *csvfp;		 // fichier de résultats (CSV)
extern FILE *inputfp;	 // Fichier d'entrée
extern int verbose_mode; // mode bavard
extern int no_zero;		 // avec ou sans zéro
extern ga_params_t ga_params;

#endif
