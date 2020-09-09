#include "matrice.h"
#include "utils.h"

int *tab_init(int dim) { return malloc(dim * sizeof(int)); }

void tab_free(int *tab) { free(tab); }

int tab_print_int(FILE *f, int *tab, int n) {
	int written = 0;
	for (int i = 0; i < n; i++) {
		if (i > 0) {
			written += fprintf(f, ", ");
		}
		written += fprintf(f, "%d", tab[i]);
	}
	return written;
}

int tab_print_hex(FILE *f, int *tab, int n) {
	int written = 0;
	for (int i = 0; i < n; i++) {
		if (i > 0) {
			written += fprintf(f, ", ");
		}
		written += fprintf(f, "%X", tab[i]);
	}
	return written;
}

int tab_print_char(FILE *f, char *tab, int n) { return fprintf(f, "%*s", n, tab); }

void tab_swap(int *tab, unsigned int i, unsigned int j) {
	int temp = tab[i];
	tab[i] = tab[j];
	tab[j] = temp;
}

void tab_reverse(int *tab, int start, int end) {
	assert(start <= end);
	int range = end - start;
	for (int i = 0; i < range / 2; i++) {
		int a = start + i;
		int b = end - i;
		tab_swap(tab, a, b);
	}
}

bool tab_next_permutation(int *tab, unsigned int n) {
	int i = n - 1;
	int j = n - 1;
	while (i > 0 && tab[i] <= tab[i - 1]) {
		i--;
	}
	if (i <= 0) {
		return false;
	}
	i--;

	while (tab[j] <= tab[i]) {
		j--;
	}

	tab_swap(tab, i, j);
	i++;

	// Inversion du suffix
	j = n - 1;
	while (i < j) {
		tab_swap(tab, i, j);
		i++;
		j--;
	}
	return true;
}

veci_t *veci_with_capacity(unsigned int capacity) {
	veci_t *vec = malloc(sizeof(veci_t));
	veci_init(vec);
	if (capacity > 0)
		vec->data = malloc(sizeof(int) * capacity);
	vec->capacity = capacity;
	vec->len = 0;
	return vec;
}

veci_t *veci_new() { return veci_with_capacity(10); }

void veci_init(veci_t *self) {
	self->capacity = 0;
	self->len = 0;
	self->data = NULL;
}

bool veci_is_init(const veci_t *self) { return self->data != NULL && self->capacity > 0; }

bool veci_is_empty(const veci_t *self) { return veci_is_init(self) || self->len == 0; }

int veci_get(const veci_t *self, unsigned int x) {
	assert(x < self->len);
	return self->data[x];
}

void veci_update_length(veci_t *self, unsigned int new_length) {
	veci_resize(self, new_length);
	self->len = new_length;
}

void veci_resize(veci_t *self, unsigned int new_capacity) {
	if (veci_is_init(self)) {
		while (self->capacity < new_capacity)
			self->capacity *= 2;
		self->data = realloc(self->data, sizeof(int) * self->capacity);
	} else {
		self->data = malloc(sizeof(int) * new_capacity);
		self->capacity = new_capacity;
		self->len = 0;
	}
}

void veci_push(veci_t *self, int val) {
	unsigned int old_len = self->len;
	veci_update_length(self, self->len + 1);
	veci_set(self, old_len, val);
}

int veci_pop(veci_t *self) {
	assert(!veci_is_empty(self));
	return self->data[self->len--];
}

void veci_set(veci_t *self, unsigned int x, int val) {
	assert(x < self->len);
	self->data[x] = val;
}

void veci_free(veci_t *self) { free(self->data); }

vecnode_t *vecnode_with_capacity(unsigned int capacity) {
	vecnode_t *self = malloc(sizeof(vecnode_t));
	vecnode_init(self);
	if (capacity > 0)
		self->data = malloc(sizeof(int) * capacity);
	self->capacity = capacity;
	self->len = 0;
	return self;
}

vecnode_t *vecnode_new() { return vecnode_with_capacity(0); }

void vecnode_init(vecnode_t *self) {
	self->len = 0;
	self->capacity = 0;
	self->data = NULL;
}

bool vecnode_is_init(const vecnode_t *self) { return self->data != NULL && self->capacity > 0; }

bool vecnode_is_empty(const vecnode_t *self) { return vecnode_is_init(self) || self->len == 0; }

node_t *vecnode_get(const vecnode_t *self, unsigned int x) {
	assert(x < self->len);
	return self->data + x;
}

void vecnode_resize(vecnode_t *self, unsigned int new_length) {
	if (new_length > self->capacity) {
		if (vecnode_is_init(self)) {
			while (self->capacity < new_length) {
				self->capacity *= 2;
			}
			self->data = realloc(self->data, sizeof(node_t) * self->capacity);
		} else {
			self->capacity = new_length;
			self->data = malloc(sizeof(node_t) * self->capacity);
		}
	}
	self->len = new_length;
}

bool veci_includes(const veci_t *self, int val) {
	bool includes = false;
	unsigned int i = 0;
	while (!includes && i < self->len) {
		includes = veci_get(self, i) == val;
		i++;
	}
	return includes;
}

void vecnode_push(vecnode_t *self, node_t *val) {
	unsigned int old_len = self->len;
	vecnode_resize(self, self->len + 1);
	self->data[old_len] = *val;
}

node_t vecnode_pop(vecnode_t *self) {
	assert(!vecnode_is_empty(self));
	return self->data[self->len--];
}

void vecnode_set(vecnode_t *self, unsigned int x, node_t *val) {
	assert(x < self->len);
	self->data[x] = *val;
}

void vecnode_free(vecnode_t *self) { free(self->data); }

static inline unsigned int idx(unsigned int cols, unsigned int rows, unsigned int x, unsigned int y) {
	assert(x < cols && y < rows);
	return cols * y + x;
}

int mati_get(mati_t *self, unsigned int x, unsigned int y) { return self->data[idx(self->cols, self->rows, x, y)]; }

void mati_set(mati_t *self, unsigned int x, unsigned int y, int val) {
	self->data[idx(self->cols, self->rows, x, y)] = val;
}

void mati_free(mati_t *self) { free(self->data); }

matd_t *matd_init(unsigned int rows, unsigned int cols) {
	matd_t *self = malloc(sizeof(matd_t));
	self->data = malloc(sizeof(double) * rows * cols);
	self->rows = rows;
	self->cols = cols;
	return self;
}

double matd_get(matd_t *self, unsigned int x, unsigned int y) { return self->data[idx(self->cols, self->rows, x, y)]; }

void matd_set(matd_t *self, unsigned int x, unsigned int y, double val) {
	self->data[idx(self->cols, self->rows, x, y)] = val;
}

void matd_free(matd_t *m) {
	free(m->data);
	free(m);
}

static inline unsigned int sym_idx(unsigned int size, unsigned int x, unsigned int y) {
	assert(x < size);
	assert(y < size);
	if (x < y) {
		return sym_idx(size, y, x);
	}
	return y * (size) + x; // FIXME: Better spatial complexity
}

matdsym_t *matdsym_new(unsigned int size) {
	matdsym_t *self = malloc(sizeof(matdsym_t));
	matdsym_init(self, size);
	return self;
}

void matdsym_init(matdsym_t *self, unsigned int size) {
	self->size = size;
	self->data = malloc(sizeof(double) * matdsym_calculate_storage_size(self));
}

unsigned int matdsym_calculate_storage_size(matdsym_t *self) {
	// return self->size * (self->size + 1) / 2;
	return self->size * self->size; // FIXME: Better spatial complexity
}

double matdsym_get(const matdsym_t *self, unsigned int x, unsigned int y) {
	unsigned int idx = sym_idx(self->size, x, y);
	return self->data[idx];
}

void matdsym_set(matdsym_t *self, unsigned int x, unsigned int y, double val) {
	unsigned int idx = sym_idx(self->size, x, y);
	self->data[idx] = val;
}

void matdsym_free(matdsym_t *self) { free(self->data); }

int *veci_begin(const veci_t *self) {
	if (veci_is_init(self) && self->len > 0)
		return self->data;
	return NULL;
}

int *veci_end(const veci_t *self) {
	if (veci_is_init(self) && self->len > 0)
		return self->data + (self->len - 1);
	return NULL;
}

const int *veci_next(const veci_t *self, const int *iter) {
	if (!iter)
		return NULL;
	long idx = iter - self->data;
	if (idx < 0 || idx >= self->len - 1)
		return NULL;
	return iter + 1;
}

node_t *vecnode_begin(const vecnode_t *self) {
	if (vecnode_is_init(self) && self->len > 0)
		return self->data;
	return NULL;
}

node_t *vecnode_end(const vecnode_t *self) {
	if (vecnode_is_init(self) && self->len > 0)
		return self->data + (self->len - 1);
	return NULL;
}

const node_t *vecnode_next(const vecnode_t *self, const node_t *cur) {
	if (!cur)
		return NULL;
	long idx = cur - self->data;
	if (idx < 0 || idx >= self->len - 1)
		return NULL;
	return cur + 1;
}

bool veci_remove(veci_t *self, int val) {
	bool deleted = false;
	const int *el_ref = veci_begin(self);
	unsigned int idx = 0;
	while (!deleted && el_ref) {
		deleted = val == *el_ref;
		el_ref = veci_next(self, el_ref);
		if (!deleted)
			idx++;
	}
	if (deleted) {
		veci_remove_at(self, idx);
	}
	return deleted;
}

void veci_remove_at(veci_t *self, unsigned int idx) {
	self->len--;
	for (unsigned int i = idx; i < self->len; i++) {
		self->data[i] = self->data[i + 1];
	}
}

mati_t *mati_new(unsigned int rows, unsigned int cols) {
	mati_t *self = malloc(sizeof(mati_t));
	mati_init(self, rows, cols);
	return self;
}

void mati_init(mati_t *self, unsigned int rows, unsigned int cols) {
	self->cols = cols;
	self->rows = rows;
	self->data = malloc(sizeof(int) * cols * rows);
}
void veci_reinit(veci_t *self) {
	if (veci_is_init(self)) {
		self->len = 0;
	} else
		veci_init(self);
}
