#include <sys/time.h>
#include <check.h>
#include "../../lib/monikor/include/metric.h"


// monikor_metric_new tests
START_TEST(test_monikor_metric_new) {
  monikor_metric_t *m;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_new("testmetric", &now);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_int_eq(now.tv_sec, m->clock.tv_sec);
  ck_assert_int_eq(now.tv_usec, m->clock.tv_usec);
  monikor_metric_free(m);
}
END_TEST


// monikor_metric_clone tests
static void assert_metric_clone(monikor_metric_t *src, monikor_metric_t *dst) {
  ck_assert_str_eq(dst->name, src->name);
  ck_assert_int_eq(dst->flags, src->flags);
  ck_assert_int_eq(dst->id, src->id);
  ck_assert_int_eq(dst->clock.tv_sec, src->clock.tv_sec);
  ck_assert_int_eq(dst->clock.tv_usec, src->clock.tv_usec);
  switch (dst->type) {
    case MONIKOR_INTEGER:
      ck_assert_int_eq(dst->value._int, src->value._int);
      break;
    case MONIKOR_FLOAT:
      ck_assert_float_eq(dst->value._float, src->value._float);
      break;
    case MONIKOR_STRING:
      ck_assert_str_eq(dst->value._string, src->value._string);
      break;
  }
}

START_TEST(test_monikor_metric_clone_int) {
  monikor_metric_t *m;
  monikor_metric_t *clone;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_integer_id("testmetric", &now, 42, MONIKOR_METRIC_AGGREGATE, 42);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_int_eq(m->clock.tv_sec, now.tv_sec);
  ck_assert_int_eq(m->clock.tv_usec, now.tv_usec);
  ck_assert_int_eq(m->value._int, 42);
  ck_assert_int_eq(m->flags, MONIKOR_METRIC_AGGREGATE);
  ck_assert_int_eq(m->id, 42);
  clone = monikor_metric_clone(m);
  assert_metric_clone(m, clone);
  monikor_metric_free(m);
  monikor_metric_free(clone);
}
END_TEST

START_TEST(test_monikor_metric_clone_float) {
  monikor_metric_t *m;
  monikor_metric_t *clone;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_float_id("testmetric", &now, 42.0, MONIKOR_METRIC_AGGREGATE, 42);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_int_eq(m->clock.tv_sec, now.tv_sec);
  ck_assert_int_eq(m->clock.tv_usec, now.tv_usec);
  ck_assert_float_eq(m->value._float, 42.0);
  ck_assert_int_eq(m->flags, MONIKOR_METRIC_AGGREGATE);
  ck_assert_int_eq(m->id, 42);
  clone = monikor_metric_clone(m);
  assert_metric_clone(m, clone);
  monikor_metric_free(m);
  monikor_metric_free(clone);
}
END_TEST

START_TEST(test_monikor_metric_clone_string) {
  monikor_metric_t *m;
  monikor_metric_t *clone;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_string_id("testmetric", &now, "stringvalue", 42);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_str_eq(m->value._string, "stringvalue");
  ck_assert_int_eq(m->clock.tv_sec, now.tv_sec);
  ck_assert_int_eq(m->clock.tv_usec, now.tv_usec);
  ck_assert_int_eq(m->flags, 0);
  ck_assert_int_eq(m->id, 42);
  clone = monikor_metric_clone(m);
  assert_metric_clone(m, clone);
  monikor_metric_free(m);
  monikor_metric_free(clone);
}
END_TEST

// monikor_metric_integer tests
START_TEST(test_monikor_metric_integer) {
  monikor_metric_t *m;
  monikor_metric_t *clone;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_integer("testmetric", &now, 42, MONIKOR_METRIC_AGGREGATE);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_int_eq(m->clock.tv_sec, now.tv_sec);
  ck_assert_int_eq(m->clock.tv_usec, now.tv_usec);
  ck_assert_int_eq(m->value._int, 42);
  ck_assert_int_eq(m->flags, MONIKOR_METRIC_AGGREGATE);
  monikor_metric_free(m);
}
END_TEST

// monikor_metric_integer_id tests
START_TEST(test_monikor_metric_integer_id) {
  monikor_metric_t *m;
  monikor_metric_t *clone;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_integer_id("testmetric", &now, 42, MONIKOR_METRIC_AGGREGATE, 42);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_int_eq(m->clock.tv_sec, now.tv_sec);
  ck_assert_int_eq(m->clock.tv_usec, now.tv_usec);
  ck_assert_int_eq(m->value._int, 42);
  ck_assert_int_eq(m->flags, MONIKOR_METRIC_AGGREGATE);
  ck_assert_int_eq(m->id, 42);
  monikor_metric_free(m);
}
END_TEST


// monikor_metric_float tests
START_TEST(test_monikor_metric_float) {
  monikor_metric_t *m;
  monikor_metric_t *clone;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_float("testmetric", &now, 42.0, MONIKOR_METRIC_AGGREGATE);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_int_eq(m->clock.tv_sec, now.tv_sec);
  ck_assert_int_eq(m->clock.tv_usec, now.tv_usec);
  ck_assert_float_eq(m->value._float, 42.0);
  ck_assert_int_eq(m->flags, MONIKOR_METRIC_AGGREGATE);
  monikor_metric_free(m);
}
END_TEST

// monikor_metric_float_id tests
START_TEST(test_monikor_metric_float_id) {
  monikor_metric_t *m;
  monikor_metric_t *clone;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_float_id("testmetric", &now, 42.0, MONIKOR_METRIC_AGGREGATE, 42);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_int_eq(m->clock.tv_sec, now.tv_sec);
  ck_assert_int_eq(m->clock.tv_usec, now.tv_usec);
  ck_assert_float_eq(m->value._float, 42.0);
  ck_assert_int_eq(m->flags, MONIKOR_METRIC_AGGREGATE);
  ck_assert_int_eq(m->id, 42);
  monikor_metric_free(m);
}
END_TEST

// monikor_metric_string tests
START_TEST(test_monikor_metric_string) {
  monikor_metric_t *m;
  monikor_metric_t *clone;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_string("testmetric", &now, "testvalue");
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_int_eq(m->clock.tv_sec, now.tv_sec);
  ck_assert_int_eq(m->clock.tv_usec, now.tv_usec);
  ck_assert_str_eq(m->value._string, "testvalue");
  monikor_metric_free(m);
}
END_TEST

// monikor_metric_string_id tests
START_TEST(test_monikor_metric_string_id) {
  monikor_metric_t *m;
  monikor_metric_t *clone;
  struct timeval now;

  now.tv_sec = 42;
  now.tv_usec = 21;
  m = monikor_metric_string_id("testmetric", &now, "testvalue", 42);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "testmetric");
  ck_assert_int_eq(m->clock.tv_sec, now.tv_sec);
  ck_assert_int_eq(m->clock.tv_usec, now.tv_usec);
  ck_assert_str_eq(m->value._string, "testvalue");
  ck_assert_int_eq(m->id, 42);
  monikor_metric_free(m);
}
END_TEST

// monikor_metric_compute_delta tests
START_TEST(test_monikor_metric_compute_delta_int) {
  monikor_metric_t *a;
  monikor_metric_t *b;
  monikor_metric_t *res;
  struct timeval clock;

  clock.tv_sec = 42;
  clock.tv_usec = 21;
  a = monikor_metric_integer("testmetric", &clock, 42, MONIKOR_METRIC_DELTA);
  ck_assert_ptr_nonnull(a);
  clock.tv_sec += 60;
  b = monikor_metric_integer("testmetric", &clock, 52, MONIKOR_METRIC_DELTA);
  ck_assert_ptr_nonnull(b);
  res = monikor_metric_compute_delta(a, b);
  ck_assert_ptr_nonnull(res);
  ck_assert_str_eq(res->name, a->name);
  ck_assert_int_eq(res->type, MONIKOR_INTEGER);
  ck_assert_int_eq(res->clock.tv_sec, a->clock.tv_sec);
  ck_assert_int_eq(res->clock.tv_usec, a->clock.tv_usec);
  ck_assert_int_eq(res->flags, 0);
  ck_assert_int_eq(res->value._int, 10);
  monikor_metric_free(res);

  a->flags = MONIKOR_METRIC_TIMEDELTA;
  b->flags = MONIKOR_METRIC_TIMEDELTA;
  res = monikor_metric_compute_delta(a, b);
  ck_assert_ptr_nonnull(res);
  ck_assert_str_eq(res->name, a->name);
  ck_assert_int_eq(res->type, MONIKOR_FLOAT);
  ck_assert_int_eq(res->clock.tv_sec, a->clock.tv_sec);
  ck_assert_int_eq(res->clock.tv_usec, a->clock.tv_usec);
  ck_assert_int_eq(res->flags, 0);
  ck_assert_int_eq(res->value._float, 10.0 / 60);
  monikor_metric_free(res);
  monikor_metric_free(a);
  monikor_metric_free(b);
}
END_TEST

START_TEST(test_monikor_metric_compute_delta_float) {
  monikor_metric_t *a;
  monikor_metric_t *b;
  monikor_metric_t *res;
  struct timeval clock;

  clock.tv_sec = 42;
  clock.tv_usec = 21;
  a = monikor_metric_float("testmetric", &clock, 42.0, MONIKOR_METRIC_DELTA);
  ck_assert_ptr_nonnull(a);
  clock.tv_sec += 60;
  b = monikor_metric_float("testmetric", &clock, 52.0, MONIKOR_METRIC_DELTA);
  ck_assert_ptr_nonnull(b);
  res = monikor_metric_compute_delta(a, b);
  ck_assert_ptr_nonnull(res);
  ck_assert_str_eq(res->name, a->name);
  ck_assert_int_eq(res->type, MONIKOR_FLOAT);
  ck_assert_int_eq(res->clock.tv_sec, a->clock.tv_sec);
  ck_assert_int_eq(res->clock.tv_usec, a->clock.tv_usec);
  ck_assert_int_eq(res->flags, 0);
  ck_assert_float_eq(res->value._float, 10.0);
  monikor_metric_free(res);

  a->flags = MONIKOR_METRIC_TIMEDELTA;
  b->flags = MONIKOR_METRIC_TIMEDELTA;
  res = monikor_metric_compute_delta(a, b);
  ck_assert_ptr_nonnull(res);
  ck_assert_str_eq(res->name, a->name);
  ck_assert_int_eq(res->type, MONIKOR_FLOAT);
  ck_assert_int_eq(res->clock.tv_sec, a->clock.tv_sec);
  ck_assert_int_eq(res->clock.tv_usec, a->clock.tv_usec);
  ck_assert_int_eq(res->flags, 0);
  ck_assert_float_eq(res->value._float, 10.0 / 60);
  monikor_metric_free(res);
  monikor_metric_free(a);
  monikor_metric_free(b);
}
END_TEST

START_TEST(test_monikor_metric_compute_delta_err) {
  monikor_metric_t *a;
  monikor_metric_t *b;
  monikor_metric_t *res;
  struct timeval clock;

  clock.tv_sec = 42;
  clock.tv_usec = 21;
  a = monikor_metric_string("testmetric", &clock, "a");
  ck_assert_ptr_nonnull(a);
  clock.tv_sec += 60;
  b = monikor_metric_string("testmetric", &clock, "b");
  ck_assert_ptr_nonnull(b);
  res = monikor_metric_compute_delta(a, b);
  ck_assert_ptr_null(res);
  monikor_metric_free(a);
  monikor_metric_free(b);

  clock.tv_sec = 42;
  clock.tv_usec = 21;
  a = monikor_metric_integer("testmetric", &clock, 42, 0);
  ck_assert_ptr_nonnull(a);
  clock.tv_sec += 60;
  b = monikor_metric_float("testmetric", &clock, 42, 0);
  ck_assert_ptr_nonnull(b);
  res = monikor_metric_compute_delta(a, b);
  ck_assert_ptr_null(res);
  monikor_metric_free(a);
  monikor_metric_free(b);
}
END_TEST


// monikor_metric_add tests
START_TEST(test_monikor_metric_add_int) {
  monikor_metric_t *a;
  monikor_metric_t *b;
  struct timeval clock;

  clock.tv_sec = 42;
  clock.tv_usec = 21;
  a = monikor_metric_integer("testmetric", &clock, 42, 0);
  b = monikor_metric_integer("testmetric", &clock, 42, 0);
  ck_assert_ptr_nonnull(a);
  ck_assert_ptr_nonnull(b);
  ck_assert_int_eq(monikor_metric_add(a, b), 0);
  ck_assert_int_eq(a->value._int, 84);
  monikor_metric_free(a);
  monikor_metric_free(b);
}
END_TEST

START_TEST(test_monikor_metric_add_float) {
  monikor_metric_t *a;
  monikor_metric_t *b;
  struct timeval clock;

  clock.tv_sec = 42;
  clock.tv_usec = 21;
  a = monikor_metric_float("testmetric", &clock, 42.0, 0);
  b = monikor_metric_float("testmetric", &clock, 42.0, 0);
  ck_assert_ptr_nonnull(a);
  ck_assert_ptr_nonnull(b);
  ck_assert_int_eq(monikor_metric_add(a, b), 0);
  ck_assert_float_eq(a->value._float, 84.0);
  monikor_metric_free(a);
  monikor_metric_free(b);
}
END_TEST

START_TEST(test_monikor_metric_add_err) {
  monikor_metric_t *a;
  monikor_metric_t *b;
  struct timeval clock;

  clock.tv_sec = 42;
  clock.tv_usec = 21;
  a = monikor_metric_integer("testmetric", &clock, 42, 0);
  b = monikor_metric_float("testmetric", &clock, 42.0, 0);
  ck_assert_ptr_nonnull(a);
  ck_assert_ptr_nonnull(b);
  ck_assert_int_ne(monikor_metric_add(a, b), 0);
  ck_assert_int_eq(a->value._int, 42);
  monikor_metric_free(a);
  monikor_metric_free(b);

  a = monikor_metric_string("testmetric", &clock, "a");
  b = monikor_metric_string("testmetric", &clock, "b");
  ck_assert_ptr_nonnull(a);
  ck_assert_ptr_nonnull(b);
  ck_assert_int_ne(monikor_metric_add(a, b), 0);
  monikor_metric_free(a);
  monikor_metric_free(b);
}
END_TEST


// monikor_metric_data_size tests
START_TEST(test_monikor_metric_data_size) {
  monikor_metric_t *m;
  struct timeval clock;

  clock.tv_sec = 42;
  clock.tv_usec = 21;
  m = monikor_metric_integer("testmetric", &clock, 42.0, 0);
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(monikor_metric_data_size(m), sizeof(uint64_t));
  monikor_metric_free(m);

  m = monikor_metric_float("testmetric", &clock, 42.0, 0);
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(monikor_metric_data_size(m), sizeof(float));
  monikor_metric_free(m);

  m = monikor_metric_string("testmetric", &clock, "test");
  ck_assert_ptr_nonnull(m);
  ck_assert_uint_eq(monikor_metric_data_size(m), 4);
  monikor_metric_free(m);
}
END_TEST

// Test suite for monikor_metric
Suite *libmonikor_metric_suite(void) {
  Suite *s = suite_create("libmonikor/metric");

  TCase *tc_metric_new = tcase_create("monikor_metric_new");
  tcase_add_test(tc_metric_new, test_monikor_metric_new);
  suite_add_tcase(s, tc_metric_new);

  TCase *tc_metric_clone = tcase_create("monikor_metric_clone");
  tcase_add_test(tc_metric_clone, test_monikor_metric_clone_int);
  tcase_add_test(tc_metric_clone, test_monikor_metric_clone_float);
  tcase_add_test(tc_metric_clone, test_monikor_metric_clone_string);
  suite_add_tcase(s, tc_metric_clone);

  TCase *tc_metric_integer = tcase_create("monikor_metric_integer");
  tcase_add_test(tc_metric_integer, test_monikor_metric_integer);
  suite_add_tcase(s, tc_metric_integer);

  TCase *tc_metric_integer_id = tcase_create("monikor_metric_integer_id");
  tcase_add_test(tc_metric_integer_id, test_monikor_metric_integer_id);
  suite_add_tcase(s, tc_metric_integer_id);

  TCase *tc_metric_float = tcase_create("monikor_metric_float");
  tcase_add_test(tc_metric_float, test_monikor_metric_float);
  suite_add_tcase(s, tc_metric_float);

  TCase *tc_metric_float_id = tcase_create("monikor_metric_float_id");
  tcase_add_test(tc_metric_float_id, test_monikor_metric_float_id);
  suite_add_tcase(s, tc_metric_float_id);

  TCase *tc_metric_string = tcase_create("monikor_metric_string");
  tcase_add_test(tc_metric_string, test_monikor_metric_string);
  suite_add_tcase(s, tc_metric_string);

  TCase *tc_metric_string_id = tcase_create("monikor_metric_string_id");
  tcase_add_test(tc_metric_string_id, test_monikor_metric_string_id);
  suite_add_tcase(s, tc_metric_string_id);

  TCase *tc_metric_compute_delta = tcase_create("monikor_metric_compute_delta");
  tcase_add_test(tc_metric_compute_delta, test_monikor_metric_compute_delta_int);
  tcase_add_test(tc_metric_compute_delta, test_monikor_metric_compute_delta_float);
  tcase_add_test(tc_metric_compute_delta, test_monikor_metric_compute_delta_err);
  suite_add_tcase(s, tc_metric_compute_delta);

  TCase *tc_metric_add = tcase_create("monikor_metric_add");
  tcase_add_test(tc_metric_add, test_monikor_metric_add_int);
  tcase_add_test(tc_metric_add, test_monikor_metric_add_float);
  tcase_add_test(tc_metric_add, test_monikor_metric_add_err);
  suite_add_tcase(s, tc_metric_add);

  TCase *tc_metric_data_size = tcase_create("monikor_metric_data_size");
  tcase_add_test(tc_metric_data_size, test_monikor_metric_data_size);
  suite_add_tcase(s, tc_metric_data_size);

  return s;
}
