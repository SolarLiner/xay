#include "tour.h"
#include "utils.h"

static int tour_cmp(tour_t *a, tour_t *b) { return SIGN(a->total_distance - b->total_distance); }

void tour_init(tour_t *tour) { tour_init_capacity(tour, 10); }

void tour_set_array(tour_t *tour, int *idx, unsigned int n) {
	// Resize vector and reset length
	veci_resize(&tour->tour, n);
	tour->tour.len = 0;

	for (int i = 0; i < n; i++) {
		tour_add(tour, idx[i]);
	}
}

void tour_from_array(tour_t *tour, int *idx, unsigned int n) {
	tour_init_capacity(tour, n);
	tour_set_array(tour, idx, n);
}

void tour_init_capacity(tour_t *tour, unsigned int capacity) {
	tour->total_distance = 0.0;
	memset(tour->name, '\0', TOUR_NAME_MAX_SIZE);
	memset(tour->method, '\0', TOUR_NAME_MAX_SIZE);
	tour->ticks = 0;
	veci_init(&tour->tour);
	veci_resize(&tour->tour, capacity);
}

void tour_dump(const tour_t *self) {
	printf("Tour %.*s", TOUR_NAME_MAX_SIZE, self->name);
	printf(", de taille %d\n", self->tour.len);
	printf("\tMéthode utilisée: %.*s\n", TOUR_NAME_MAX_SIZE, self->method);
	printf("\tLongueur calculée %lf\n", self->total_distance);
	printf("\tTemps de calcul: %3.3lf ms\n", 1e3 * self->ticks / CLOCKS_PER_SEC);
	printf("\tComposée des nœuds:\n\t\t");
	print_array_int(self->tour.data, self->tour.len);
	printf("\n");
}

void tour_hardcopy(tour_t *dest, const tour_t *src) {
	tour_init(dest);
	veci_resize(&dest->tour, src->tour.capacity);
	dest->total_distance = src->total_distance;
	strncpy(dest->name, src->name, TOUR_NAME_MAX_SIZE);
	strncpy(dest->method, src->method, TOUR_NAME_MAX_SIZE);
	tour_set_array(dest, veci_begin(&src->tour), src->tour.len);
}

bool tour_has_node(tour_t *tour, int node) {
	bool has_tour = false;
	int i = 0;
	while (i < tour->tour.len && !has_tour) {
		has_tour = veci_get(&tour->tour, i) == node;
		i++;
	}
	return has_tour;
}

void tour_add(tour_t *tour, int node) {
	if (tour_has_node(tour, node))
		return;
	veci_push(&tour->tour, node);
}

bool tour_nodes_adjacent(tour_t *tour, int n1, int n2) {
	bool adjacent = false;
	int i = 0;
	while (i < tour_dimension(tour) - 1 && !adjacent) {
		adjacent = (veci_get(&tour->tour, i) == n1 && veci_get(&tour->tour, i + 1) == n2) ||
				   (veci_get(&tour->tour, i) == n2 && veci_get(&tour->tour, i + 1) == n1);
		i++;
	}
	return adjacent;
}
unsigned int tour_dimension(const tour_t *self) { return self->tour.len; }

void tour_dump_csv(FILE *file, tour_t *self) {
	fprintf(file, "Méthode; Longueur; Temps CPU (s); Tour\n");
	fprintf(file, "%.*s; %lf; %lf; ", TOUR_NAME_MAX_SIZE, self->method, self->total_distance,
			(double)self->ticks / CLOCKS_PER_SEC);
	if (no_zero)
		fprintf(csvfp, "[");
	else
		fprintf(file, "[0");
	for (unsigned int i = 0; i < self->tour.len; i++) {
		if ((no_zero && i > 0) || !no_zero)
			fprintf(file, ", ");
		fprintf(file, "%d", self->tour.data[i] + 1);
	}
	fprintf(file, "]\n");
}

void tour_sort(tour_t *arr, unsigned int n) { qsort(arr, n, sizeof(tour_t), (__compar_fn_t)tour_cmp); }

void tour_free(tour_t *self) { veci_free(&self->tour); }
