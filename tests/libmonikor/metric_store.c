#include <stdlib.h>
#include <sys/time.h>
#include <check.h>
#include "../../lib/monikor/include/metric.h"

START_TEST(test_monikor_metric_store_new) {
  monikor_metric_store_t *s;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);
  ck_assert_ptr_nonnull(s->current);
  ck_assert_ptr_nonnull(s->cache);
  ck_assert_ptr_nonnull(s->delta);
  ck_assert_uint_eq(s->current->size, 0);
  ck_assert_uint_eq(s->cache->size, 0);
  ck_assert_uint_eq(s->delta->size, 0);
  monikor_metric_store_free(s);
}
END_TEST

START_TEST(test_monikor_metric_store_push) {
  monikor_metric_store_t *s;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);
  ret = monikor_metric_store_push(s, monikor_metric_integer("testmetric", &clock, 42, 0));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 1);
  ck_assert_uint_eq(s->cache->size, 0);
  ck_assert_uint_eq(s->delta->size, 0);
  monikor_metric_store_free(s);
}

START_TEST(test_monikor_metric_store_push_delta) {
  monikor_metric_store_t *s;
  monikor_metric_t *m;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);

  // Push one delta and one timedelta metric
  ret = monikor_metric_store_push(s, monikor_metric_integer("delta", &clock, 42, MONIKOR_METRIC_DELTA));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 0);
  ck_assert_uint_eq(s->delta->size, 1);
  ret = monikor_metric_store_push(s, monikor_metric_integer("timedelta", &clock, 42, MONIKOR_METRIC_TIMEDELTA));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 0);
  ck_assert_uint_eq(s->delta->size, 2);

  // Push the same metrics again 1 minute later to get the deltas computed
  clock.tv_sec += 60;
  ret = monikor_metric_store_push(s, monikor_metric_integer("delta", &clock, 126, MONIKOR_METRIC_DELTA));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 1);
  ck_assert_uint_eq(s->delta->size, 2);
  ret = monikor_metric_store_push(s, monikor_metric_integer("timedelta", &clock, 126, MONIKOR_METRIC_TIMEDELTA));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 2);
  ck_assert_uint_eq(s->delta->size, 2);

  // Check output metrics
  m = monikor_metric_list_pop(s->current);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "delta");
  ck_assert_uint_eq(m->flags, 0);
  ck_assert_uint_eq(m->type, MONIKOR_INTEGER);
  ck_assert_uint_eq(m->value._int, 84);
  monikor_metric_free(m);
  m = monikor_metric_list_pop(s->current);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "timedelta");
  ck_assert_uint_eq(m->flags, 0);
  ck_assert_uint_eq(m->type, MONIKOR_FLOAT);
  ck_assert_float_eq(m->value._float, 84.0 / 60.0);
  monikor_metric_free(m);

  // Check delta metrics update
  m = monikor_metric_list_pop(s->delta);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "delta");
  ck_assert_uint_eq(m->value._int, 126);
  monikor_metric_free(m);
  m = monikor_metric_list_pop(s->delta);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "timedelta");
  ck_assert_uint_eq(m->value._int, 126);
  monikor_metric_free(m);

  monikor_metric_store_free(s);
}

START_TEST(test_monikor_metric_store_push_delta_ids) {
  monikor_metric_store_t *s;
  monikor_metric_t *m;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);

  // Push some metrics with different ids
  ret = monikor_metric_store_push(s, monikor_metric_integer_id("delta", &clock, 42, MONIKOR_METRIC_DELTA, 1));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 0);
  ck_assert_uint_eq(s->delta->size, 1);
  ret = monikor_metric_store_push(s, monikor_metric_integer_id("delta", &clock, 1, MONIKOR_METRIC_DELTA, 2));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 0);
  ck_assert_uint_eq(s->delta->size, 2);

  // Push second metric again, it should match with the id and thus we have 1 current metric with id 2
  clock.tv_sec += 60;
  ret = monikor_metric_store_push(s, monikor_metric_integer_id("delta", &clock, 5, MONIKOR_METRIC_DELTA, 2));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 1);
  ck_assert_uint_eq(s->delta->size, 2);
  m = s->current->first->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(m->id, 2);
  ck_assert_uint_eq(m->value._int, 4);
  m = s->delta->last->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(m->id, 2);
  ck_assert_uint_eq(m->value._int, 5);

  monikor_metric_store_free(s);
}
END_TEST


START_TEST(test_monikor_metric_store_push_aggregate) {
  monikor_metric_store_t *s;
  monikor_metric_t *m;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);

  // Push one aggregate metric
  ret = monikor_metric_store_push(s, monikor_metric_integer("testmetric", &clock, 42, MONIKOR_METRIC_AGGREGATE));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 1);
  ck_assert_uint_eq(s->cache->size, 0);
  ck_assert_uint_eq(s->delta->size, 0);
  m = s->current->first->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(m->value._int, 42);

  // Push the same metric again 1 minute later
  clock.tv_sec += 60;
  ret = monikor_metric_store_push(s, monikor_metric_integer("testmetric", &clock, 42, MONIKOR_METRIC_AGGREGATE));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 1);
  m = s->current->first->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(m->value._int, 84);

  monikor_metric_store_free(s);
}

START_TEST(test_monikor_metric_store_push_aggregate_ids) {
  monikor_metric_store_t *s;
  monikor_metric_t *m;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);

  // Push 2 aggregate metrics with different ids
  ret = monikor_metric_store_push(s, monikor_metric_integer_id("testmetric", &clock, 42, MONIKOR_METRIC_AGGREGATE, 1));
  ck_assert_int_eq(ret, 0);
  ret = monikor_metric_store_push(s, monikor_metric_integer_id("testmetric", &clock, 42, MONIKOR_METRIC_AGGREGATE, 2));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 2);
  ck_assert_uint_eq(s->cache->size, 0);
  ck_assert_uint_eq(s->delta->size, 0);
  m = s->current->first->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(m->value._int, 42);

  // Push one metric 1 minute later, it should be aggregated with the one with same id
  clock.tv_sec += 60;
  ret = monikor_metric_store_push(s, monikor_metric_integer_id("testmetric", &clock, 42, MONIKOR_METRIC_AGGREGATE, 2));
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(s->current->size, 2);
  m = s->current->first->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(m->id, 1);
  ck_assert_uint_eq(m->value._int, 42);
  m = s->current->last->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(m->id, 2);
  ck_assert_uint_eq(m->value._int, 84);

  monikor_metric_store_free(s);
}


START_TEST(test_monikor_metric_store_lpush) {
  monikor_metric_store_t *s;
  monikor_metric_list_t *l;
  monikor_metric_t *m;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);
  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(l);

  monikor_metric_list_push(l, monikor_metric_integer("a", &clock, 42, 0));
  monikor_metric_list_push(l, monikor_metric_integer("b", &clock, 84, 0));
  monikor_metric_list_push(l, monikor_metric_integer("c", &clock, 126, 0));
  ck_assert_uint_eq(l->size, 3);
  ck_assert_uint_eq(s->current->size, 0);
  ret = monikor_metric_store_lpush(s, l);
  ck_assert_int_eq(ret, 0);
  ck_assert_uint_eq(l->size, 0);
  ck_assert_uint_eq(s->current->size, 3);
  m = s->current->first->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "a");
  ck_assert_uint_eq(m->value._int, 42);

  monikor_metric_list_free(l);
  monikor_metric_store_free(s);
}

START_TEST(test_monikor_metric_store_flush) {
  monikor_metric_store_t *s;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);
  monikor_metric_list_push(s->current, monikor_metric_integer("cur", &clock, 42, 0));
  monikor_metric_list_push(s->delta, monikor_metric_integer("delta", &clock, 42, 0));
  monikor_metric_list_push(s->cache, monikor_metric_integer("cache", &clock, 42, 0));
  ck_assert_uint_eq(s->current->size, 1);
  ck_assert_uint_eq(s->delta->size, 1);
  ck_assert_uint_eq(s->cache->size, 1);
  monikor_metric_store_flush(s);
  ck_assert_uint_eq(s->current->size, 0);
  ck_assert_uint_eq(s->delta->size, 1);
  ck_assert_uint_eq(s->cache->size, 1);

  monikor_metric_store_free(s);
}

START_TEST(test_monikor_metric_store_flush_all) {
  monikor_metric_store_t *s;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);
  monikor_metric_list_push(s->current, monikor_metric_integer("cur", &clock, 42, 0));
  monikor_metric_list_push(s->delta, monikor_metric_integer("delta", &clock, 42, 0));
  monikor_metric_list_push(s->cache, monikor_metric_integer("cache", &clock, 42, 0));
  ck_assert_uint_eq(s->current->size, 1);
  ck_assert_uint_eq(s->delta->size, 1);
  ck_assert_uint_eq(s->cache->size, 1);
  monikor_metric_store_flush_all(s);
  ck_assert_uint_eq(s->current->size, 0);
  ck_assert_uint_eq(s->delta->size, 0);
  ck_assert_uint_eq(s->cache->size, 0);

  monikor_metric_store_free(s);
}

START_TEST(test_monikor_metric_store_cache) {
  monikor_metric_store_t *s;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);
  monikor_metric_list_push(s->current, monikor_metric_integer("testmetric", &clock, 42, 0));
  ck_assert_uint_eq(s->current->size, 1);
  ck_assert_uint_eq(s->cache->size, 0);
  monikor_metric_store_cache(s);
  ck_assert_uint_eq(s->current->size, 0);
  ck_assert_uint_eq(s->cache->size, 1);

  monikor_metric_store_free(s);
}

START_TEST(test_monikor_metric_store_uncache) {
  monikor_metric_store_t *s;
  monikor_metric_t *m;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);
  monikor_metric_list_push(s->current, monikor_metric_integer("current", &clock, 42, 0));
  monikor_metric_list_push(s->cache, monikor_metric_integer("cache", &clock, 42, 0));
  ck_assert_uint_eq(s->current->size, 1);
  ck_assert_uint_eq(s->cache->size, 1);
  monikor_metric_store_uncache(s);
  ck_assert_uint_eq(s->current->size, 2);
  ck_assert_uint_eq(s->cache->size, 0);

  // Check that uncached metrics appears first in the store
  m = monikor_metric_list_pop(s->current);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "cache");
  monikor_metric_free(m);
  m = monikor_metric_list_pop(s->current);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "current");
  monikor_metric_free(m);

  monikor_metric_store_free(s);
}

START_TEST(test_monikor_metric_store_evict_cache) {
  monikor_metric_store_t *s;
  monikor_metric_t *m;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;
  size_t max_size;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);
  monikor_metric_list_push(s->cache, monikor_metric_integer("a", &clock, 42, 0));
  max_size = s->cache->data_size + 1; // We want to retain only one metric
  monikor_metric_list_push(s->cache, monikor_metric_integer("b", &clock, 42, 0));
  monikor_metric_list_push(s->cache, monikor_metric_integer("c", &clock, 42, 0));
  ret = monikor_metric_store_evict_cache(s, max_size);
  ck_assert_int_eq(ret, 2);
  ck_assert_uint_eq(s->cache->size, 1);

  // Only last inserted metric is there
  m = s->cache->first->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "c");

  monikor_metric_store_free(s);
}

START_TEST(test_monikor_metric_store_evict_delta) {
  monikor_metric_store_t *s;
  monikor_metric_t *m;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  int ret;

  s = monikor_metric_store_new();
  ck_assert_ptr_nonnull(s);
  monikor_metric_list_push(s->delta, monikor_metric_integer("a", &clock, 42, 0));
  monikor_metric_list_push(s->delta, monikor_metric_integer("b", &clock, 42, 0));
  clock.tv_sec += 120;
  monikor_metric_list_push(s->delta, monikor_metric_integer("c", &clock, 42, 0));
  monikor_metric_list_push(s->delta, monikor_metric_integer("d", &clock, 42, 0));
  clock.tv_sec -= 60;
  ret = monikor_metric_store_evict_delta(s, &clock);
  ck_assert_int_eq(ret, 2);
  ck_assert_uint_eq(s->delta->size, 2);
  m = s->delta->first->metric;
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "c");

  monikor_metric_store_free(s);
}


Suite *libmonikor_metric_store_suite(void) {
  Suite *s = suite_create("libmonikor/metric_store");

  TCase *tc_metric_store_new = tcase_create("monikor_metric_store_new");
  tcase_add_test(tc_metric_store_new, test_monikor_metric_store_new);
  suite_add_tcase(s, tc_metric_store_new);

  TCase *tc_metric_store_push = tcase_create("monikor_metric_store_push");
  tcase_add_test(tc_metric_store_push, test_monikor_metric_store_push);
  tcase_add_test(tc_metric_store_push, test_monikor_metric_store_push_delta);
  tcase_add_test(tc_metric_store_push, test_monikor_metric_store_push_delta_ids);
  tcase_add_test(tc_metric_store_push, test_monikor_metric_store_push_aggregate);
  tcase_add_test(tc_metric_store_push, test_monikor_metric_store_push_aggregate_ids);
  suite_add_tcase(s, tc_metric_store_push);

  TCase *tc_metric_store_lpush = tcase_create("monikor_metric_store_lpush");
  tcase_add_test(tc_metric_store_lpush, test_monikor_metric_store_lpush);
  suite_add_tcase(s, tc_metric_store_lpush);

  TCase *tc_metric_store_flush = tcase_create("monikor_metric_store_flush");
  tcase_add_test(tc_metric_store_flush, test_monikor_metric_store_flush);
  suite_add_tcase(s, tc_metric_store_flush);

  TCase *tc_metric_store_flush_all = tcase_create("monikor_metric_store_flush_all");
  tcase_add_test(tc_metric_store_flush_all, test_monikor_metric_store_flush_all);
  suite_add_tcase(s, tc_metric_store_flush_all);

  TCase *tc_metric_store_cache = tcase_create("monikor_metric_store_cache");
  tcase_add_test(tc_metric_store_cache, test_monikor_metric_store_cache);
  suite_add_tcase(s, tc_metric_store_cache);

  TCase *tc_metric_store_uncache = tcase_create("monikor_metric_store_uncache");
  tcase_add_test(tc_metric_store_uncache, test_monikor_metric_store_uncache);
  suite_add_tcase(s, tc_metric_store_uncache);

  TCase *tc_metric_store_evict_cache = tcase_create("monikor_metric_store_evict_cache");
  tcase_add_test(tc_metric_store_evict_cache, test_monikor_metric_store_evict_cache);
  suite_add_tcase(s, tc_metric_store_evict_cache);

  TCase *tc_metric_store_evict_delta = tcase_create("monikor_metric_store_evict_delta");
  tcase_add_test(tc_metric_store_evict_delta, test_monikor_metric_store_evict_delta);
  suite_add_tcase(s, tc_metric_store_evict_delta);

  return s;
}
