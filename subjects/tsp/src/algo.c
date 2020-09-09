//
// Created by solarliner on 04/01/2020.
//

#include "algo.h"

#define GA_SELECTION_BIAS 10

void instance_tour_ppv(instance_t *self, tour_t *tour) {
	node_t zero = {0};
	veci_t nodes_idx;
	unsigned int dim = instance_dimension(self);
	veci_init(&nodes_idx);
	tour_init(tour);
	strncpy(tour->name, self->name, TOUR_NAME_MAX_SIZE);
	strncpy(tour->method, "PPV", TOUR_NAME_MAX_SIZE);

	instance_build_distmat(self);
	if (verbose_mode)
		instance_dump_matdist(self);
	for (unsigned int n = 0; n < dim; n++) {
		double curr_min_dist;
		int curr_min_idx;

		if (n == 0) {
			curr_min_dist = node_distance(&zero, vecnode_get(&self->nodes, 0));
			curr_min_idx = 0;
			for (int i = 1; i < dim; i++) {
				double dist = node_distance(&zero, vecnode_get(&self->nodes, i));
				if (dist < curr_min_dist) {
					curr_min_dist = dist;
					curr_min_idx = i;
				}
			}
		} else {
			int prev_idx = veci_get(&nodes_idx, n - 1);
			bool has_min = false;
			for (int i = 0; i < dim; i++) {
				if (veci_includes(&nodes_idx, i))
					continue;
				double dist = instance_dist_leg(self, prev_idx, i);
				if (!has_min) {
					curr_min_dist = dist;
					curr_min_idx = i;
					has_min = true;
				} else if (dist < curr_min_dist) {
					curr_min_dist = dist;
					curr_min_idx = i;
				}
			}
			assert(has_min);
		}
		veci_push(&nodes_idx, curr_min_idx);
		tour->total_distance += curr_min_dist;
	}
	tour->total_distance += node_distance(&zero, vecnode_get(&self->nodes, *veci_end(&nodes_idx)));
	tour_set_array(tour, nodes_idx.data, nodes_idx.len);
}

void instance_tour_random_walk(instance_t *self, tour_t *result) {
	veci_t nodes_left;
	const unsigned int dimension = instance_dimension(self);
	tour_init_capacity(result, dimension);
	veci_init(&nodes_left);

	strncpy(result->name, self->name, TOUR_NAME_MAX_SIZE);
	strncpy(result->method, "RW", TOUR_NAME_MAX_SIZE);

	for (int i = 0; i < dimension; i++) {
		veci_push(&nodes_left, i);
	}

	while (nodes_left.len > 0) {
		int idx = RANDOM_CHOICE(nodes_left.data, nodes_left.len);
		tour_add(result, idx);
		veci_remove(&nodes_left, idx);
	}

	instance_tour_calc_length(self, result, false);
}

void instance_tour_swap_2opt(const instance_t *self, tour_t *tour, unsigned int i, unsigned int j) {
	assert(instance_dimension(self) == tour_dimension(tour));
	const unsigned int dim = instance_dimension(self);
	veci_t vec;
	const unsigned int min = MIN(i + 1, j);
	const unsigned int max = MAX(i + 1, j);
	veci_init(&vec);
	for (unsigned int cur = 0; cur < min; cur++) {
		veci_push(&vec, veci_get(&tour->tour, cur));
	}
	for (unsigned int cur = min; cur < max; cur++) {
		const unsigned int c = cur - min;
		const unsigned int idx = max - 1 - c;
		veci_push(&vec, veci_get(&tour->tour, idx));
	}
	for (unsigned int cur = max; cur < dim; cur++) {
		veci_push(&vec, veci_get(&tour->tour, cur));
	}
	tour_set_array(tour, vec.data, vec.len);
	instance_tour_calc_length(self, tour, false);
	veci_free(&vec);
}

bool instance_improve_2opt(instance_t *self, tour_t *tour) {
	bool modified = false;
	const unsigned int dim = instance_dimension(self);
	for (unsigned int i = 0; i < dim - 1; i++) {
		for (unsigned j = 0; j < dim - 1; j++) {
			if (j == i - 1 || j == i || j == i + 1)
				continue;
			node_t *ni = vecnode_get(&self->nodes, veci_get(&tour->tour, i));
			node_t *ni1 = vecnode_get(&self->nodes, veci_get(&tour->tour, i + 1));
			node_t *nj = vecnode_get(&self->nodes, veci_get(&tour->tour, j));
			node_t *nj1 = vecnode_get(&self->nodes, veci_get(&tour->tour, j + 1));
			if ((node_distance(ni, ni1) + node_distance(nj, nj1)) > (node_distance(ni, nj) + node_distance(ni1, nj1))) {
				tour_t tmp;
				tour_from_array(&tmp, tour->tour.data, tour->tour.len);
				instance_tour_swap_2opt(self, &tmp, i + 1, j);
				if (tmp.total_distance < tour->total_distance) {
					tour_set_array(tour, tmp.tour.data, tmp.tour.len);
					tour->total_distance = tmp.total_distance;
					modified = true;
				}
				tour_free(&tmp);
			}
		}
	}
	instance_tour_calc_length(self, tour, false);
	return modified;
}

void instance_tour_2opt(instance_t *self, tour_t *tour, const bool use_ppv) {
	char filename[MAXBUF];
#ifdef WITH_CAIRO
	unsigned int i = 1;
#endif
	if (use_ppv)
		instance_tour_ppv(self, tour);
	else
		instance_tour_random_walk(self, tour);
	strncpy(tour->method, "2-OPT", TOUR_NAME_MAX_SIZE);
	if (use_ppv)
		strncat(tour->method, " (w/ PPV)", TOUR_NAME_MAX_SIZE);
	else
		strncat(tour->method, " (w/ RW)", TOUR_NAME_MAX_SIZE);
	while (instance_improve_2opt(self, tour)) {
#ifdef WITH_CAIRO
		if (verbose_mode) {
			snprintf(filename, MAXBUF, "2opt_%04d.png", i++);
			draw_instance_tour(self, tour, filename);
		}
#endif
	}
}

void instance_tour_produce_crossover(const instance_t *self, tour_t *dest, const tour_t *a, const tour_t *b) {
	assert(instance_dimension(self) == tour_dimension(a) && instance_dimension(self) == tour_dimension(b));
	tour_hardcopy(dest, a);
	for (unsigned int i = 0; i < tour_dimension(dest); i++) {
		const int idx_a = veci_get(&a->tour, i);
		const int idx_b = veci_get(&b->tour, i);
		if (idx_a == idx_b || veci_includes(&dest->tour, idx_b)) {
			veci_set(&dest->tour, i, idx_a);
		} else if (veci_includes(&dest->tour, idx_a)) {
			veci_set(&dest->tour, i, idx_b);
		} else {
			veci_set(&dest->tour, i, RANDOM_EITHER(idx_a, idx_b));
		}
	}
	instance_tour_calc_length(self, dest, false);
}

void prepare_prob_array(double *arr, const tour_t *population) {
	double max_distance = population[0].total_distance;
	double sum = 0.0;
	for (unsigned int i = 1; i < ga_params.population; i++) {
		if (max_distance < population[i].total_distance)
			max_distance = population[i].total_distance;
	}

	for (unsigned int i = 0; i < ga_params.population; i++) {
		arr[i] = pow(max_distance - population[i].total_distance, GA_SELECTION_BIAS);
		sum += arr[i];
	}

	for (unsigned int i = 0; i < ga_params.population; i++) {
		arr[i] /= sum;
	}
}

void tour_get_parents(const tour_t *population, const double *prob_array, tour_t const **a, tour_t const **b) {
	unsigned int idx_a = rand_prob(prob_array, ga_params.population);
	unsigned int idx_b;
	idx_b = rand_prob(prob_array, ga_params.population);
	if (idx_a == idx_b) {
		if (idx_a == 0)
			idx_b = 1;
		else if (idx_a == ga_params.population - 1)
			idx_b = idx_a - 1;
		else
			idx_b = idx_a + RANDOM_EITHER(-1, 1);
	}
	(*a) = population + idx_a;
	(*b) = population + idx_b;
}

void tour_mutate(const instance_t *self, tour_t *tour) {
	if (RANDOM_DOUBLE() < ga_params.mutation) {
		const unsigned int dim = tour_dimension(tour);
		const unsigned int idx_a = RANDOM_RANGE(0, dim);
		unsigned int idx_b;
		do {
			idx_b = RANDOM_RANGE(0, dim);
		} while (idx_b == idx_a);
		instance_tour_swap_2opt(self, tour, RANDOM_RANGE(0, dim), 0);
	}
}

void instance_tour_genetic(instance_t *self, tour_t *tour) {
	tour_t *population = malloc(sizeof(tour_t) * ga_params.population);
	const unsigned int breeding = ga_params.population / 2;
	tour_t *children = malloc(sizeof(tour_t) * breeding);
	double *p = malloc(sizeof(double) * ga_params.population);
	for (unsigned int i = 0; i < ga_params.population; i++) {
		tour_t *t = population + i;
		tour_init(t);
		instance_tour_random_walk(self, t);
		strncpy(t->name, self->name, TOUR_NAME_MAX_SIZE);
		if (ga_params.with_2opt)
			strncpy(t->method, "GA + 2OPT", TOUR_NAME_MAX_SIZE);
		else
			strncpy(t->method, "GA", TOUR_NAME_MAX_SIZE);
	}

	tour_sort(population, ga_params.population);
	for (unsigned int epoch = 0; epoch < ga_params.epochs; epoch++) {
		prepare_prob_array(p, population);
		for (unsigned int j = 0; j < breeding; j++) {
			tour_t *a;
			tour_t *b;
			tour_get_parents(population, p, &a, &b);

			tour_t *child = children + j;
			instance_tour_produce_crossover(self, child, a, b);
			tour_mutate(self, child);
			if (ga_params.with_2opt)
				while (instance_improve_2opt(self, child))
					;
		}
		for (unsigned int j = 0; j < breeding; j++) {
			unsigned int idx = ga_params.population - 1 - j;
			population[idx] = children[j];
		}

		tour_sort(population, ga_params.population);
#ifdef WITH_CAIRO
		if (verbose_mode) {
			char filename[MAXBUF];
			snprintf(filename, MAXBUF, "ga_e%04d.png", epoch);
			draw_instance_tour(self, population, filename);
		}
#endif
		printf("\n------- GEN %5d -------\n", epoch + 1);
		tour_dump(population);
	}

	tour_hardcopy(tour, population);
	printf("\n------ END RESULT -------\n");
	for (unsigned int i = 0; i < ga_params.population; i++) {
		tour_dump(population + i);
		tour_free(population + i);
	}

	free(p);
	free(children);
	free(population);
}

void instance_tour_bruteforce(instance_t *self, tour_t *best, tour_t *worst, const bool use_mat) {
	tour_t current_tour;
	bool first_tour = true;
	const unsigned int dimension = instance_dimension(self);

	if (verbose_mode)
		fprintf(stderr, "Précalcul des distances...\n");
	if (use_mat) {
		instance_build_distmat(self);
		if (verbose_mode)
			instance_dump_matdist(self);
	}
	tour_init(&current_tour);
	strncpy(current_tour.name, self->name, TOUR_NAME_MAX_SIZE);
	for (int i = 0; i < dimension; i++) {
		tour_add(&current_tour, i);
	}
	instance_tour_calc_length(self, &current_tour, use_mat);

	while (tab_next_permutation(current_tour.tour.data, dimension)) {
		/*if(verbose_mode) {
		  printf("Permutation: ");
		  print_array_int(current_tour.tour.data, current_tour.tour.len);
		  printf("\n");
		  }*/
		if (first_tour) {
			tour_hardcopy(best, &current_tour);
			tour_hardcopy(worst, &current_tour);
			first_tour = false;
		} else {
			double length = instance_tour_calc_length(self, &current_tour, use_mat);
			if (length < best->total_distance) {
				tour_hardcopy(best, &current_tour);
			}
			if (length > worst->total_distance) {
				tour_hardcopy(worst, &current_tour);
			}
		}
	}

	strncpy(best->method, "BF (meilleur)", TOUR_NAME_MAX_SIZE);
	strncpy(worst->method, "BF (pire)", TOUR_NAME_MAX_SIZE);
	tour_free(&current_tour);
}
