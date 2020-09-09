#include "cli.h"
#include "utils.h"

void get_args(int argc, char **argv, bool tab_arg[NB_BALISES]) {
	int i = 0;
	bool fin_lecture = false;
	// Mise à zéro du tableau de sortie
	for (i = 0; i < NB_BALISES; i++) {
		tab_arg[i] = false;
	}

	if (argc == 1) {
		fprintf(stderr, "Nombre d'arguements invalide.\n");
		print_help(argv[0]);
		return;
	}

	// Récupération des arguements
	i = 1;
	while (i < argc && !fin_lecture) {
		if (streq(argv[i], "-h")) {
			tab_arg[BAL_H] = true;
			print_help(argv[0]);
			fin_lecture = true;
		} else if (streq(argv[i], "-nz")) {
			no_zero = true;
		}
#ifdef WITH_CAIRO
		else if (streq(argv[i], "-draw")) {
			tab_arg[BAL_DRAW] = true;
		}
#endif
		else if (streq(argv[i], "-f")) {
			if (prefix("-", argv[i + 1]))
				error("-f: Fichier attendu\n");
			else {
#ifndef TESTS
				fprintf(stderr, "Fichier TSP: %.100s\n", argv[i + 1]);
#endif
				inputfp = fopen(argv[++i], "r");
				tab_arg[BAL_F] = true;
			}

		} else if (streq(argv[i], "-t")) {
			if (i + 1 >= argc || prefix("-", argv[i + 1]))
				error("-t: Fichier attendu\n");
			else {
#ifndef TESTS
				fprintf(stderr, "Fichier solution: %.100s", argv[i + 1]);
#endif
				tourfp = fopen(argv[++i], "w");
				tab_arg[BAL_T] = true;
			}
		} else if (streq(argv[i], "-v")) {
			verbose_mode = 1;
			tab_arg[BAL_V] = true;
			if (i + 1 >= argc || prefix("-", argv[i + 1]))
				logfp = fopen("log.txt", "w");
			else
				logfp = fopen(argv[++i], "w");
		} else if (streq(argv[i], "-o")) {
			if (i + 1 >= argc || prefix("-", argv[i + 1]))
				error("-o: Fichier attendu\n");
			else {
				csvfp = fopen(argv[++i], "w");
				tab_arg[BAL_O] = true;
			}
		} else if (streq(argv[i], "-ppv"))
			tab_arg[BAL_PPV] = true;
		else if (streq(argv[i], "-bf"))
			tab_arg[BAL_BF] = true;
		else if (streq(argv[i], "-bfm"))
			tab_arg[BAL_BFM] = true;
		else if (streq(argv[i], "-rw"))
			tab_arg[BAL_RW] = true;
		else if (streq(argv[i], "-2opt"))
			tab_arg[BAL_2OPT] = true;
		else if (streq(argv[i], "-ga")) {
			tab_arg[BAL_GA] = true;
			if (parse_param_int(&ga_params.population, argv[i + 1])) {
				i++;
				if (parse_param_int(&ga_params.epochs, argv[i + 1])) {
					i++;
					if (parse_param_double(&ga_params.mutation, argv[i + 1])) {
						i++;
					}
				}
			}
		} else if (streq(argv[i], "-ga-2opt")) {
			ga_params.with_2opt = true;
		}

		i++;
	}
}

void print_help(char *argv0) {
#ifndef TESTS
	printf("Utilisation: %.100s -f <fichier> [-t <tour>] [-v [<fichier>]] "
		   "-<méthode> [-h]\n",
		   argv0);
	printf("-f <file> : nom du fichier tsp (obligatoire)\n"
		   "-t <file> : nom du fichier solution (optionnel)\n"
		   "-v [<file>] : mode verbose (optionnel), écrit dans un fichier si "
		   "présent\n"
		   "-o <file> : export des résultats sans un fichier csv\n"
		   "-h : help, affiche ce message");
	printf("\n<méthode> : méthodes de calcul (cumulables) :\n\n");
	printf("-bf : brute force,\n"
		   "-bfm : brute force en utilisant la matrice de distance,\n"
		   "-ppv : plus proche voisin,\n"
		   "-rw : random walk,\n"
		   "-2opt : 2 optimisation. Si -ppv ou -rw ne sont pas présentes on "
		   "utilise "
		   "-rw,\n"
		   "-ga <nombre d'individus> <nombre de générations> <taux de "
		   "mutation> :\n"
		   "algorithme génétique, défaut = 20 individus, 200 générations, 0.3 "
		   "mutation.\n"
		   "-ga-2opt: Optimise chaque individu avec une 2-opt\n");
#endif
}
