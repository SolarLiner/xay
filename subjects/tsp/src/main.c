#include "algo.h"
#include "cli.h"
#ifdef WITH_CAIRO
#include "draw.h"
#endif
#include "instance.h"
#include "utils.h"
#include <time.h>

int main(int argc, char **argv) {
	instance_t inst;
	bool args[NB_BALISES];
	srand((unsigned)time(NULL));
	get_args(argc, argv, args);

	if (!inputfp)
		error("Un fichier valide est requis.\n");
	instance_read_file(&inst, inputfp);
	if (verbose_mode)
		instance_dump(&inst);
	if (csvfp)
		instance_dump_csv(csvfp, &inst);

	if (args[BAL_BF] || args[BAL_BFM]) {
		tour_t best, worst;
		clock_t start, end;

		start = clock();
		instance_tour_bruteforce(&inst, &best, &worst, args[BAL_BFM]);
		end = clock();
		best.ticks = worst.ticks = end - start;
		printf("\n----------- MEILLEUR TOUR --------\n");
		tour_dump(&best);

		printf("\n------------- PIRE TOUR ----------\n");
		tour_dump(&worst);

		if (csvfp) {
			fprintf(csvfp, "\n");
			tour_dump_csv(csvfp, &best);
			fprintf(csvfp, "\n");
			tour_dump_csv(csvfp, &worst);
		}
#ifdef WITH_CAIRO
		if (args[BAL_DRAW]) {
			draw_instance_tour(&inst, &best, "bf_best.png");
			draw_instance_tour(&inst, &worst, "bf_worst.png");
		}
#endif
		tour_free(&best);
		tour_free(&worst);
	}

	if (args[BAL_PPV] /*&& !args[BAL_2OPT]*/) {
		tour_t result;
		clock_t start, end;

		start = clock();
		instance_tour_ppv(&inst, &result);
		end = clock();
		result.ticks = end - start;
		printf("\n--------- PLUS-PETIT VOISIN ------\n");
		tour_dump(&result);
		if (csvfp) {
			fprintf(csvfp, "\n");
			tour_dump_csv(csvfp, &result);
		}

#ifdef WITH_CAIRO
		if (args[BAL_DRAW])
			draw_instance_tour(&inst, &result, "ppv.png");
#endif
		tour_free(&result);
	}

	if (args[BAL_RW] /*&& !args[BAL_2OPT]*/) {
		tour_t result;
		clock_t start, end;

		start = clock();
		instance_tour_random_walk(&inst, &result);
		end = clock();

		result.ticks = end - start;

		printf("\n---------- MARCHE ALÉATOIRE ------\n");
		tour_dump(&result);
		if (csvfp) {
			fprintf(csvfp, "\n");
			tour_dump_csv(csvfp, &result);
		}
#ifdef WITH_CAIRO
		if (args[BAL_DRAW])
			draw_instance_tour(&inst, &result, "rw.png");
#endif
		tour_free(&result);
	}

	if (args[BAL_2OPT]) {
		if (!args[BAL_PPV] && !args[BAL_RW]) {
			error("Vous devez choisir un algorithme de base pour le 2-opt: -ppv ou -rw.");
		} else {
			tour_t result;
			clock_t start, end;

			start = clock();
			instance_tour_2opt(&inst, &result, args[BAL_PPV]);
			end = clock();
			result.ticks = end - start;

			printf("\n----------- 2-OPTIMISATION -------\n");
			tour_dump(&result);
			if (csvfp) {
				fprintf(csvfp, "\n");
				tour_dump_csv(csvfp, &result);
			}
#ifdef WITH_CAIRO
			if (args[BAL_DRAW])
				draw_instance_tour(&inst, &result, "2opt.png");
#endif
			tour_free(&result);
		}
	}

	if (args[BAL_GA]) {
		tour_t result;
		clock_t start, end;

		start = clock();
		instance_tour_genetic(&inst, &result);
		end = clock();
		result.ticks = end - start;

		printf("\n----------- ALGO GENETIQUE -------\n"
			   "\tParamètres:\n");
		printf("\t\tPopulation       : %d\n", ga_params.population);
		printf("\t\tGénérations      : %d\n", ga_params.epochs);
		printf("\t\tTaux de mutations: %lf\n", ga_params.mutation);
		printf("\t\tAvec 2-opt       : %4s\n", ga_params.with_2opt ? "vrai" : "faux");

		tour_dump(&result);

#ifdef WITH_CAIRO
		if (args[BAL_DRAW])
			draw_instance_tour(&inst, &result, "ga.png");
#endif
		if (csvfp) {
			fprintf(csvfp, "\n");
			tour_dump_csv(csvfp, &result);
		}
		tour_free(&result);
	}

	instance_free(&inst);
	printf("\n");
	return 0;
}
