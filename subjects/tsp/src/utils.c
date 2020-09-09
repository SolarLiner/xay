#include "utils.h"
#include "instance.h"
#include "node.h"

void error(char *message) {
	fprintf(stderr, "E: %.100s\n", message);
#ifndef TESTS
	exit(1);
#endif
}

int prefix(char *motif, char *buf) {
	size_t motif_len = strlen(motif);
	return strncmp(motif, buf, motif_len) == 0;
}

void copy_array_int(int *dest, int *src, unsigned int n) {
	for (int i = 0; i < n; i++) {
		dest[i] = src[i];
	}
}

void print_array_int(int *tab, unsigned int n) { fprint_array_int(stdout, tab, n); }

void print_array_node(node_t *tab, unsigned int n) { fprint_array_node(stdout, tab, n); }

bool read_line(FILE *file, int n, char *output) {
	int c = fgetc(file);
	int i = 0;

	while (c != '\n' && c != EOF) {
		if (i < n - 1)
			output[i++] = c;
		c = getc(file);
	}
	output[i] = '\0';
	return c != EOF;
}

bool streq(char *a, char *b) { return strcmp(a, b) == 0; }

bool double_eq(double a, double b) { return fabs(a - b) < EPSILON; }

regex_t compile_regex(const char *input, int flags) {
	regex_t result;
	int status;
	if ((status = regcomp(&result, input, flags))) {
		char errbuf[MAXBUF];
		regerror(status, &result, errbuf, MAXBUF);
		error(errbuf);
	}
	return result;
}
void fprint_array_int(FILE *file, int *tab, unsigned int n) {
	fprintf(file, "[");
	for (unsigned int i = 0; i < n; i++) {
		if (i > 0)
			fprintf(file, ", ");
		fprintf(file, "%d", tab[i]);
	}
	fprintf(file, "]");
}
void fprint_array_node(FILE *file, node_t *tab, unsigned int n) {
	fprintf(file, "[");
	for (int i = 0; i < n; i++) {
		if (i > 0)
			fprintf(file, ", ");
		fprint_node(file, tab + i);
	}
	fprintf(file, "]");
}
char *string_from_match(const char *input, regmatch_t *match) {
	size_t range = match->rm_eo - match->rm_so;
	char *out = malloc(sizeof(char) * (range + 1));
	strncpy(out, input + match->rm_so, range);
	out[range] = '\0';
	return out;
}

unsigned int rand_prob(const double *p, const unsigned int n) {
	double s = RANDOM_DOUBLE();
	unsigned int res = 0;
	do {
		s -= p[res++];
	} while (res < n - 1 && s >= 0);
	return res;
}

void print_array_double(const double *arr, unsigned int n) { fprint_array_double(stdout, arr, n); }

void fprint_array_double(FILE *file, const double *arr, unsigned int n) {
	fprintf(file, "[");
	for (unsigned int i = 0; i < n; i++) {
		if (i > 0)
			fprintf(file, ", ");
		fprintf(file, "%lf", arr[i]);
	}
	fprintf(file, "]");
}

bool parse_param_int(unsigned int *dest, const char *input) {
	if (!input)
		return false;
	bool has_value;
	char *end;
	errno = 0;
	unsigned int res = (unsigned int)strtol(input, &end, 10);
	has_value = !errno && end > input;
	if (has_value)
		*dest = res;
	return has_value;
}

bool parse_param_double(double *dest, const char *input) {
	if (!input)
		return false;
	bool has_value;
	char *end;
	errno = 0;
	double res = strtod(input, &end);
	has_value = !errno && end > input;
	if (has_value)
		*dest = res;
	return has_value;
}
