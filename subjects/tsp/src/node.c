#include "node.h"
#include "utils.h"

typedef enum orientation_e {
	ORIENTATION_COLINEAR,
	ORIENTATION_CLOCKWISE,
	ORIENTATION_COUNTER_CLOCKWISE,
} orientation_t;

void print_node(node_t *node) { fprint_node(stdout, node); }

double node_distance(node_t *a, node_t *b) {
	double x = b->x - a->x;
	double y = b->y - a->y;

	return sqrt(x * x + y * y);
}

bool node_eq(const node_t *a, const node_t *b) { return double_eq(a->x, b->x) && double_eq(a->y, b->y); }

void fprint_node(FILE *file, node_t *self) { fprintf(file, "(%.2lf, %.2lf)", self->x, self->y); }

bool on_segment(const node_t *p, const node_t *q, const node_t *r) {
	return q->x <= MAX(p->x, r->x) && q->x >= MIN(p->x, r->x) && q->y <= MAX(p->y, r->y) && q->y >= MIN(p->y, r->y);
}

orientation_t orientation(const node_t *p, const node_t *q, const node_t *r) {
	double val = (q->y - p->y) * (r->x - q->x) - (q->x - p->x) * (r->y - q->y);
	if ((int)val == 0)
		return ORIENTATION_COLINEAR;
	return (val > 0) ? ORIENTATION_CLOCKWISE : ORIENTATION_COUNTER_CLOCKWISE;
}

bool line_intersect(const node_t *a1, const node_t *a2, const node_t *b1, const node_t *b2) {
	orientation_t o1 = orientation(a1, a2, b1), o2 = orientation(a1, b1, b2), o3 = orientation(a2, b2, a1),
				  o4 = orientation(a2, b2, b1);
	if (o1 != o2 && o3 != o4)
		return true;

	if (o1 == ORIENTATION_COLINEAR && on_segment(a1, a2, b1))
		return true;
	if (o2 == ORIENTATION_COLINEAR && on_segment(a1, b2, b1))
		return true;
	if (o3 == ORIENTATION_COLINEAR && on_segment(a2, a1, b2))
		return true;
	if (o4 == ORIENTATION_COLINEAR && on_segment(a2, b1, b2))
		return true;
	return false;
}
