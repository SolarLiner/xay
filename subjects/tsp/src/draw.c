//
// Created by solarliner on 31/12/2019.
//
#include "draw.h"
#include "utils.h"

#define MARGIN 2.5e-2
#define LINE_WIDTH 2e-3
#define POINT_SIZE 1e-2
#define FONT_SIZE 1.5 * POINT_SIZE
#define M_TAU M_PI * 2.0

#define IMG_WIDTH 2000
#define IMG_HEIGHT 2000

#ifdef HAS_CAIRO

void cairo_show_text_center(cairo_t *cr, char *text) {
  cairo_text_extents_t te;
  cairo_text_extents(cr, text, &te);

  cairo_save(cr);

  cairo_rel_move_to(cr, -te.width / 2.0, te.height / 2.0);
  cairo_show_text(cr, text);

  cairo_restore(cr);
}

void hsv_to_rgb(double h, double s, double v, double *r, double *g, double *b) {
  double hh, p, q, t, ff;
  long i;

  if (s <= 0.0) {
    *r = *g = *b = v;
  } else {
    hh = h;
    if (hh >= 360.0)
      hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = FRACT(hh);
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch (i) {
    case 0:
      *r = v;
      *g = t;
      *b = p;
      break;
    case 1:
      *r = q;
      *g = v;
      *b = p;
      break;
    case 2:
      *r = p;
      *g = v;
      *b = t;
      break;
    case 3:
      *r = p;
      *g = q;
      *b = v;
      break;
    case 4:
      *r = t;
      *g = p;
      *b = v;
      break;
    case 5:
    default:
      *r = v;
      *g = p;
      *b = q;
    }
  }
}

vecnode_t nodes_to_surface_space(vecnode_t *input) {
  vecnode_t result;
  vecnode_init(&result);

  const node_t *nref = vecnode_begin(input);
  node_t min = {
      .x = nref->x,
      .y = nref->y,
  };
  node_t max = {
      .x = nref->x,
      .y = nref->y,
  };

  nref = vecnode_next(input, nref);
  while (nref) {
    if (nref->x < min.x)
      min.x = nref->x;
    if (nref->y < min.y)
      min.y = nref->y;
    if (nref->x > max.x)
      max.x = nref->x;
    if (nref->y > max.y)
      max.y = nref->y;
    nref = vecnode_next(input, nref);
  }

  nref = vecnode_begin(input);
  while (nref) {
    double normx = (nref->x - min.x) / (max.x - min.x);
    double normy = (nref->y - min.y) / (max.y - min.y);
    node_t node = {
        .x = MARGIN + (1.0 - 2.0 * MARGIN) * normx,
        .y = MARGIN + (1.0 - 2.0 * MARGIN) * normy,
    };

    vecnode_push(&result, &node);
    nref = vecnode_next(input, nref);
  }

  return result;
}

void draw_instance(cairo_surface_t *surface, instance_t *instance) {
  vecnode_t ss_nodes = nodes_to_surface_space(&instance->nodes);
  if (verbose_mode) {
    printf("[DEBUG] Screen-space nodes: ");
    print_array_node(ss_nodes.data, ss_nodes.len);
    printf("\n");
  }
  cairo_t *cr = cairo_create(surface);
  int width = cairo_image_surface_get_width(surface);
  int height = cairo_image_surface_get_height(surface);
  int sizing = MIN(width, height);
  const node_t *nref = vecnode_begin(&ss_nodes);

  cairo_select_font_face(cr, "Ubuntu Mono", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, sizing * FONT_SIZE);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

  while (nref) {
    char buf[4];
    snprintf(buf, 4, "%ld", nref - vecnode_begin(&ss_nodes));
    double sx = width * nref->x;
    double sy = height * nref->y;
    cairo_move_to(cr, sx, sy);
    cairo_arc(cr, sx, sy, sizing * POINT_SIZE, 0.0, M_TAU);
    cairo_set_source_rgb(cr, 0.1, 0.2, 1.0);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, sx, sy);
    cairo_show_text_center(cr, buf);
    nref = vecnode_next(&ss_nodes, nref);
  }
  cairo_destroy(cr);
  cairo_surface_flush(surface);
}

void draw_tour(cairo_surface_t *surface, instance_t *instance, tour_t *tour) {
  vecnode_t ss_nodes = nodes_to_surface_space(&instance->nodes);
  cairo_t *cr = cairo_create(surface);
  double angle = 0.0, angle_step = 360.0 / ss_nodes.len, r, g, b;
  int width = cairo_image_surface_get_width(surface),
      height = cairo_image_surface_get_height(surface);
  int sizing = MIN(width, height);
  const int *idx = veci_begin(&tour->tour);
  node_t *vec = vecnode_get(&ss_nodes, *idx);
  cairo_set_line_width(cr, LINE_WIDTH * sizing);
  if (!no_zero) {
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, vec->x * width, vec->y * height);
    cairo_line_to(cr, 0, 0);
    cairo_line_to(cr, vec->x, vec->y);
    vec = vecnode_get(&ss_nodes, instance_dimension(instance) - 1);

    cairo_stroke(cr);
  }
  cairo_move_to(cr, vec->x * width, vec->y * height);

  idx = veci_next(&tour->tour, idx);
  while (idx) {
    vec = vecnode_get(&ss_nodes, *idx);
    hsv_to_rgb(angle, 0.9, 0.7, &r, &g, &b);
    cairo_set_source_rgb(cr, r, g, b);
    cairo_line_to(cr, vec->x * width, vec->y * height);
    cairo_stroke(cr);
    cairo_move_to(cr, vec->x * width, vec->y * height);

    angle += angle_step;
    idx = veci_next(&tour->tour, idx);
  }
  cairo_destroy(cr);
  cairo_surface_flush(surface);
}

void draw_instance_tour(instance_t *inst, tour_t *tour, char *filename) {
  cairo_surface_t *surf =
      cairo_image_surface_create(CAIRO_FORMAT_RGB24, IMG_WIDTH, IMG_HEIGHT);
  {
    cairo_t *cr = cairo_create(surf);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, 0.0, 0.0, IMG_WIDTH, IMG_HEIGHT);
    cairo_fill(cr);
    cairo_destroy(cr);
  }
  draw_tour(surf, inst, tour);
  draw_instance(surf, inst);
  cairo_surface_write_to_png(surf, filename);
  cairo_surface_destroy(surf);
}

#endif