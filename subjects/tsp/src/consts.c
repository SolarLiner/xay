#include "consts.h"

FILE *logfp = NULL;
FILE *csvfp = NULL;
FILE *tourfp = NULL;
FILE *inputfp = NULL;
int verbose_mode = 0;
int no_zero = 0;
ga_params_t ga_params = {
	.population = 200,
	.with_2opt = false,
	.epochs = 1000,
	.mutation = 0.3,
};
