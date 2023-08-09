#include <stdlib.h>

#include "metric.h"


static float _compute_metrics_interval(monikor_metric_t *a, monikor_metric_t *b) {
  float duration;

  duration = 1000000 * (b->clock.tv_sec - a->clock.tv_sec)
    + (b->clock.tv_usec - a->clock.tv_usec);
  return duration / 1000000.0;
}

static void _compute_float_delta(monikor_metric_t *a, monikor_metric_t *b, monikor_metric_t *res) {
  res->type = MONIKOR_FLOAT;
  res->value._float = b->value._float - a->value._float;
  if ((a->flags & MONIKOR_METRIC_TIMEDELTA) == MONIKOR_METRIC_TIMEDELTA) {
    if (b->value._float < a->value._float)
      res->value._float = 0.0;
    else
      res->value._float /= _compute_metrics_interval(a, b);
  }
}

static void _compute_int_delta(monikor_metric_t *a, monikor_metric_t *b, monikor_metric_t *res) {
  if ((a->flags & MONIKOR_METRIC_TIMEDELTA) == MONIKOR_METRIC_TIMEDELTA) {
    res->type = MONIKOR_FLOAT;
    if (b->value._int < a->value._int)
      res->value._float = 0.0;
    else
      res->value._float = (float)(b->value._int - a->value._int) / _compute_metrics_interval(a, b);
  } else {
    res->type = MONIKOR_INTEGER;
    if (b->value._int < a->value._int)
      res->value._int = 0;
    else
      res->value._int = b->value._int - a->value._int;
  }
}

monikor_metric_t *monikor_metric_compute_delta(monikor_metric_t *a, monikor_metric_t *b) {
  monikor_metric_t *result;

  if (a->type != b->type)
    return NULL;
  if (!(result = monikor_metric_clone(a)))
    return NULL;
  result->flags &= ~(MONIKOR_METRIC_DELTA | MONIKOR_METRIC_TIMEDELTA);
  switch (a->type) {
  case MONIKOR_INTEGER:
    _compute_int_delta(a, b, result);
    break;
  case MONIKOR_FLOAT:
    _compute_float_delta(a, b, result);
    break;
  default:
    monikor_metric_free(result);
    return NULL;
  }
  return result;
}

int monikor_metric_add(monikor_metric_t *dst, const monikor_metric_t *src) {
  if (dst->type != src->type)
    return 1;
  if (dst->type == MONIKOR_INTEGER)
    dst->value._int += src->value._int;
  else if (dst->type == MONIKOR_FLOAT)
    dst->value._float += src->value._float;
  else
    return 1;
  return 0;
}
