#include "instance.h"
#include "utils.h"
#ifdef WITH_CAIRO
#include "algo.h"
#include "draw.h"
#endif

bool regex_match_line_format(char *input, char *type, char *value) {
	regex_t regex = compile_regex("([a-z0-9_]+)\\s*:\\s*(.+)\\s*", REG_ICASE | REG_EXTENDED);
	int status;
	regmatch_t reg_match[3];

	status = regexec(&regex, input, 3, reg_match, 0);
	regfree(&regex);
	if (!status) {
		char *const first_match = string_from_match(input, reg_match + 1);
		strcpy(type, first_match);
		char *const second_match = string_from_match(input, reg_match + 2);
		strcpy(value, second_match);
		free(first_match);
		free(second_match);
	}
	return !status;
}

void instance_init(instance_t *self) {
	vecnode_init(&self->nodes);
	memset(&self->distances, 0, sizeof(matdsym_t));
	strncpy(self->name, "\0", TOUR_NAME_MAX_SIZE);
	strncpy(self->type, "\0", TOUR_NAME_MAX_SIZE);
	strncpy(self->edge_type, "\0", TOUR_NAME_MAX_SIZE);
	self->total_distance = 0.0;
}

void instance_dump(instance_t *self) {
	printf("name: %.*s\n", TOUR_NAME_MAX_SIZE, self->name);
	printf("type: %.*s\n", TOUR_NAME_MAX_SIZE, self->type);
	printf("dimension : %d\n", instance_dimension(self));
	printf("edge type: %.*s\n", TOUR_NAME_MAX_SIZE, self->edge_type);
	printf("Stored tour total_distance: %lf\n", self->total_distance);
	printf("Tour indices: ");
	print_array_node(self->nodes.data, self->nodes.len);
	printf("\n");
}

void instance_dump_legdist(instance_t *self, unsigned int n1, unsigned int n2) {
	printf("dist %d <-> %d: %lf\n", n1, n2, matdsym_get(&self->distances, n1, n2));
}

void instance_dump_matdist(const instance_t *self) {
	for (unsigned int y = 0; y < instance_dimension(self); y++) {
		for (unsigned int x = 0; x < instance_dimension(self); x++) {
			if (x < y)
				printf("         ");
			else {
				printf("% 5.2lf ", matdsym_get(&self->distances, x, y));
			}
		}
		printf("\n");
	}
}

double instance_dist_leg(instance_t *self, unsigned int i1, unsigned int i2) {
	return matdsym_get(&self->distances, i1, i2);
}

double instance_dist_mat(instance_t *self, unsigned int *indices, int n) {
	double total = 0.;
	for (int i = 1; i < n; i++) {
		total += instance_dist_leg(self, indices[i - 1], indices[i]);
	}
	return total;
}

unsigned int instance_dimension(const instance_t *self) { return self->nodes.len; }

void instance_build_distmat(instance_t *self) {
	unsigned int dimension = instance_dimension(self);
	matdsym_init(&self->distances, dimension);
	for (unsigned int j = 0; j < dimension; j++)
		for (unsigned int i = j; i < dimension; i++) {
			if (i == j)
				matdsym_set(&self->distances, i, j, 0);
			else
				matdsym_set(&self->distances, i, j,
							node_distance(vecnode_get(&self->nodes, i), vecnode_get(&self->nodes, j)));
		}
}

void instance_read_file(instance_t *self, FILE *file) {
	instance_init(self);
	unsigned int expected_dimension = 0;
	char buf[MAXBUF];
	char type[MAXBUF];
	char value[MAXBUF];
	while (read_line(file, MAXBUF, buf)) {
		if (regex_match_line_format(buf, type, value)) {
			if (streq("NAME", type))
				strncpy(self->name, value, TOUR_NAME_MAX_SIZE);
			else if (streq("TYPE", type)) {
				if (!streq("TSP", value)) {
					error("Ce programme ne supporte que les problèmes de TSP.");
					return;
				}
				strncpy(self->type, value, TOUR_NAME_MAX_SIZE);
			} else if (streq("DIMENSION", type))
				expected_dimension = atoi(value);
			else if (streq("EDGE_WEIGHT_TYPE", type)) {
				if (verbose_mode && !streq("EUC_2D", value))
					fprintf(stderr,
							"Ce programme ne considère que les problèmes en 2D "
							"et ignore "
							"cet indicateur: EDGE_WEIGHT_TYPE: %s\n",
							value);
				strncpy(self->edge_type, value, TOUR_NAME_MAX_SIZE);
			}
		} else if (prefix("EOF", buf))
			break;
		else if (prefix("NODE_COORD_SECTION", buf)) {
			bool cont = read_line(file, MAXBUF, buf);
			while (cont && !prefix("EOF", buf)) {
				int nidx;
				double x, y;
				node_t node;
				sscanf(buf, "%d %lf %lf", &nidx, &x, &y);
				node.x = x;
				node.y = y;
				vecnode_push(&self->nodes, &node);
				cont = read_line(file, MAXBUF, buf);
			}
		}
	}
	if (instance_dimension(self) != expected_dimension)
		error("Le fichier n'a pas autant de nœuds que annoncé");
}

void instance_free(instance_t *self) {
	vecnode_free(&self->nodes);
	// mati_free(&self->visited_nodes);
	matdsym_free(&self->distances);
}

double instance_tour_calc_length(const instance_t *self, tour_t *tour, const bool use_mat) {
	assert(instance_dimension(self) == tour_dimension(tour));
	double length = 0.0;
	unsigned int dimension = instance_dimension(self);
	if (!no_zero) {
		node_t zero = {0};
		length = node_distance(&zero, vecnode_get(&self->nodes, veci_get(&tour->tour, 0))) +
				 node_distance(vecnode_get(&self->nodes, veci_get(&tour->tour, instance_dimension(self) - 1)), &zero);
	}
	for (unsigned int i = 0; i < dimension - 1; i++) {
		unsigned int i1 = veci_get(&tour->tour, i);
		unsigned int i2 = veci_get(&tour->tour, i + 1);
		if (use_mat)
			length += matdsym_get(&self->distances, i1, i2);
		else
			length += node_distance(vecnode_get(&self->nodes, i1), vecnode_get(&self->nodes, i2));
	}
	tour->total_distance = length;
	return length;
}

void instance_dump_csv(FILE *file, instance_t *self) {
	fprintf(file, "Nom de l'instance; %.*s\n", TOUR_NAME_MAX_SIZE, self->name);
	fprintf(file, "Nombre de villes; %d\n", instance_dimension(self));
	fprintf(file, "Type; %.*s\n", TOUR_NAME_MAX_SIZE, self->type);
	fprintf(file, "Point; Abscisse; Ordonnée;\n");
	if (!no_zero)
		fprintf(file, "0;     0;     0\n");
	for (unsigned int i = 0; i < self->nodes.len; i++) {
		node_t *node = self->nodes.data + i;
		fprintf(file, "%d; %3.2lf; %3.2lf\n", i + 1, node->x, node->y);
	}
}
