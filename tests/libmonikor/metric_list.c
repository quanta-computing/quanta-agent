#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <check.h>
#include "../../lib/monikor/include/metric.h"


START_TEST(test_monikor_metric_list_init) {
  monikor_metric_list_t l;

  l.size = 42;
  l.data_size = 42;
  l.first = (monikor_metric_list_node_t *)42;
  l.last = l.first;
  monikor_metric_list_init(&l);
  ck_assert_uint_eq(l.size, 0);
  ck_assert_uint_eq(l.data_size, 0);
  ck_assert_ptr_null(l.first);
  ck_assert_ptr_null(l.last);
}
END_TEST


START_TEST(test_monikor_metric_list_new) {
  monikor_metric_list_t *l;

  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(l);
  ck_assert_uint_eq(l->size, 0);
  ck_assert_uint_eq(l->data_size, 0);
  monikor_metric_list_free(l);
}
END_TEST


START_TEST(test_monikor_metric_list_empty) {
  monikor_metric_t *m;
  monikor_metric_list_t *l;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };

  m = monikor_metric_integer("testmetric", &clock, 42, 0);
  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(m);
  ck_assert_ptr_nonnull(l);
  monikor_metric_list_push(l, m);
  ck_assert_uint_eq(l->size, 1);
  ck_assert_uint_gt(l->data_size, 0);
  ck_assert_ptr_eq(l->first->metric, m);
  monikor_metric_list_empty(l);
  ck_assert_uint_eq(l->size, 0);
  ck_assert_uint_eq(l->data_size, 0);
  ck_assert_ptr_null(l->first);
  ck_assert_ptr_null(l->last);
  monikor_metric_list_free(l);
}
END_TEST

START_TEST(test_monikor_metric_list_push) {
  monikor_metric_t *m;
  monikor_metric_list_t *l;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };

  m = monikor_metric_integer("testmetric", &clock, 42, 0);
  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(m);
  ck_assert_ptr_nonnull(l);
  monikor_metric_list_push(l, m);
  ck_assert_uint_eq(l->size, 1);
  ck_assert_uint_gt(l->data_size, 0);
  ck_assert_ptr_eq(l->first->metric, m);
  monikor_metric_list_free(l);
}
END_TEST

START_TEST(test_monikor_metric_list_pop) {
  monikor_metric_t *m1;
  monikor_metric_t *m2;
  monikor_metric_list_t *l;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };

  m1 = monikor_metric_integer("testmetric", &clock, 42, 0);
  m2 = monikor_metric_integer("testmetric", &clock, 42, 0);
  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(m1);
  ck_assert_ptr_nonnull(m2);
  ck_assert_ptr_nonnull(l);
  monikor_metric_list_push(l, m1);
  monikor_metric_list_push(l, m2);
  ck_assert_uint_eq(l->size, 2);
  ck_assert_ptr_eq(l->first->metric, m1);
  ck_assert_ptr_eq(l->last->metric, m2);
  ck_assert_ptr_eq(monikor_metric_list_pop(l), m1);
  ck_assert_uint_eq(l->size, 1);
  ck_assert_ptr_eq(monikor_metric_list_pop(l), m2);
  ck_assert_uint_eq(l->size, 0);
  ck_assert_uint_eq(l->data_size, 0);
  ck_assert_ptr_null(l->first);
  ck_assert_ptr_null(l->last);
  monikor_metric_list_free(l);
  monikor_metric_free(m1);
  monikor_metric_free(m2);
}
END_TEST


typedef struct {
  int count;
} test_monikor_metric_list_iterator_data_t;

void test_monikor_metric_list_apply_f(monikor_metric_t *m, void *data) {
  test_monikor_metric_list_iterator_data_t *state = (test_monikor_metric_list_iterator_data_t *)data;

  ck_assert_ptr_nonnull(m);
  ++state->count;
  if (state->count == 1) {
    ck_assert_str_eq(m->name, "m1");
  } else if (state->count == 2) {
    ck_assert_str_eq(m->name, "m2");
  }
}

START_TEST(test_monikor_metric_list_apply) {
  test_monikor_metric_list_iterator_data_t state = { .count = 0 };
  monikor_metric_list_t *l;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };

  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(l);
  monikor_metric_list_push(l, monikor_metric_integer("m1", &clock, 42, 0));
  monikor_metric_list_push(l, monikor_metric_integer("m2", &clock, 42, 0));
  monikor_metric_list_apply(l, &test_monikor_metric_list_apply_f, (void *)&state);
  ck_assert_int_eq(state.count, 2);
  monikor_metric_list_free(l);
}
END_TEST


START_TEST(test_monikor_metric_list_concat) {
  monikor_metric_list_t *a;
  monikor_metric_list_t *b;
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };

  a = monikor_metric_list_new();
  b = monikor_metric_list_new();
  ck_assert_ptr_nonnull(a);
  ck_assert_ptr_nonnull(b);
  monikor_metric_list_push(b, monikor_metric_integer("m1", &clock, 42, 0));
  ck_assert_uint_eq(a->size, 0);
  ck_assert_uint_eq(b->size, 1);
  monikor_metric_list_concat(a, b);
  ck_assert_uint_eq(a->size, 1);
  ck_assert_uint_eq(b->size, 0);
  monikor_metric_list_concat(a, b);
  ck_assert_uint_eq(a->size, 1);
  ck_assert_uint_eq(b->size, 0);
  monikor_metric_list_push(b, monikor_metric_integer("m2", &clock, 42, 0));
  ck_assert_uint_eq(a->size, 1);
  ck_assert_uint_eq(b->size, 1);
  monikor_metric_list_concat(a, b);
  ck_assert_uint_eq(a->size, 2);
  ck_assert_uint_eq(b->size, 0);
  monikor_metric_list_free(a);
  monikor_metric_list_free(b);
}
END_TEST

static int test_monikor_metric_list_remove_if_f(monikor_metric_t *m, __attribute__((unused)) void *data) {
  test_monikor_metric_list_iterator_data_t *state = (test_monikor_metric_list_iterator_data_t *)data;

  ++state->count;
  return !strcmp(m->name, "rm");
}

START_TEST(test_monikor_metric_list_remove_if) {
  test_monikor_metric_list_iterator_data_t state = { .count = 0 };
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  monikor_metric_list_t *l;
  monikor_metric_t *m;
  int removed;

  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(l);
  monikor_metric_list_push(l, monikor_metric_integer("rm", &clock, 42, 0));
  monikor_metric_list_push(l, monikor_metric_integer("m1", &clock, 42, 0));
  monikor_metric_list_push(l, monikor_metric_integer("rm", &clock, 42, 0));
  monikor_metric_list_push(l, monikor_metric_integer("m2", &clock, 42, 0));
  ck_assert_int_eq(l->size, 4);
  removed = monikor_metric_list_remove_if(l, &test_monikor_metric_list_remove_if_f, (void *)&state);
  ck_assert_int_eq(state.count, 4);
  ck_assert_int_eq(removed, 2);
  ck_assert_int_eq(l->size, 2);
  m = monikor_metric_list_pop(l);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "m1");
  monikor_metric_free(m);
  m = monikor_metric_list_pop(l);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "m2");
  monikor_metric_free(m);
  monikor_metric_list_free(l);
}
END_TEST


START_TEST(test_monikor_metric_list_serialize) {
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  monikor_metric_list_t *l;
  void *data;
  size_t size;

  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(l);
  monikor_metric_list_push(l, monikor_metric_integer("m1", &clock, 42, 0));
  ck_assert_int_eq(monikor_metric_list_serialize(l, &data, &size), 0);
  ck_assert_uint_eq(size, 49);

  free(data);
  monikor_metric_list_free(l);
}


START_TEST(test_monikor_metric_list_serialize_max_count) {
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  monikor_metric_list_t *l;
  monikor_serialized_metric_list_hdr_t hdr;
  void *data;
  size_t size;

  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(l);
  for (size_t i = 0; i < METRIC_LIST_MAX_COUNT; i++)
    monikor_metric_list_push(l, monikor_metric_integer("m", &clock, i, 0));
  ck_assert_int_eq(monikor_metric_list_serialize(l, &data, &size), 0);
  monikor_metric_list_header_unserialize(data, &hdr);
  ck_assert_uint_eq(hdr.count, METRIC_LIST_MAX_COUNT);
  free(data);

  monikor_metric_list_push(l, monikor_metric_integer("m", &clock, 0, 0));
  ck_assert_int_ne(monikor_metric_list_serialize(l, &data, &size), 0);
  ck_assert_ptr_null(data);
  monikor_metric_list_free(l);
}
END_TEST


START_TEST(test_monikor_metric_list_unserialize) {
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  void *data = NULL;
  size_t size = 0;
  monikor_metric_list_t *l;
  monikor_serialized_metric_list_hdr_t hdr;
  monikor_metric_list_t *metrics;
  monikor_metric_t *m;

  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(l);
  monikor_metric_list_push(l, monikor_metric_integer("m1", &clock, 42, MONIKOR_METRIC_AGGREGATE));
  ck_assert_int_eq(monikor_metric_list_serialize(l, &data, &size), 0);
  ck_assert_uint_eq(size, 49);
  ck_assert_ptr_nonnull(data);

  monikor_metric_list_header_unserialize(data, &hdr);
  ck_assert_uint_eq(hdr.version, 1);
  ck_assert_uint_eq(hdr.count, 1);
  ck_assert_uint_eq(hdr.data_size, 38);
  ck_assert_uint_eq(monikor_metric_list_unserialize(data + SERIALIZED_METRIC_LIST_HDR_SIZE, &hdr, &metrics), 1);
  ck_assert_uint_eq(metrics->size, 1);
  m = monikor_metric_list_pop(metrics);
  ck_assert_ptr_nonnull(m);
  ck_assert_str_eq(m->name, "m1");
  ck_assert_uint_eq(m->clock.tv_sec, 42);
  ck_assert_uint_eq(m->clock.tv_usec, 21);
  ck_assert_uint_eq(m->flags, MONIKOR_METRIC_AGGREGATE);
  ck_assert_uint_eq(m->value._int, 42);

  free(data);
  monikor_metric_free(m);
  monikor_metric_list_free(metrics);
}
END_TEST


static size_t unserialize_tampered(monikor_metric_t *m, uint16_t count, uint64_t data_size,
int name_size, uint64_t metric_data_size) {
  monikor_metric_list_t *l;
  monikor_serialized_metric_list_hdr_t hdr;
  monikor_metric_list_t *metrics;
  void *data;
  uint8_t *payload;
  size_t size;
  size_t ret;

  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(l);
  monikor_metric_list_push(l, m);
  ck_assert_int_eq(monikor_metric_list_serialize(l, &data, &size), 0);
  monikor_metric_list_free(l);
  monikor_metric_list_header_unserialize(data, &hdr);
  hdr.count = count;
  if (data_size < hdr.data_size)
    hdr.data_size = data_size;
  // exact-size copy so any read past data_size leaves the allocation
  payload = malloc(hdr.data_size);
  ck_assert_ptr_nonnull(payload);
  memcpy(payload, (uint8_t *)data + SERIALIZED_METRIC_LIST_HDR_SIZE, hdr.data_size);
  if (name_size >= 0)
    *((uint16_t *)(payload + METRIC_HDR_NAME_SIZE_OFF)) = htons((uint16_t)name_size);
  if (metric_data_size)
    *((uint64_t *)(payload + METRIC_HDR_DATA_SIZE_OFF)) = htonll(metric_data_size);
  ret = monikor_metric_list_unserialize(payload, &hdr, &metrics);
  ck_assert_ptr_nonnull(metrics);
  ck_assert_uint_eq(metrics->size, ret);
  monikor_metric_list_free(metrics);
  free(payload);
  free(data);
  return ret;
}

START_TEST(test_monikor_metric_list_unserialize_err) {
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };

  // sanity: untampered integer and string records are accepted
  ck_assert_uint_eq(unserialize_tampered(monikor_metric_integer("m1", &clock, 42, 0),
    1, UINT64_MAX, -1, 0), 1);
  ck_assert_uint_eq(unserialize_tampered(monikor_metric_string("m1", &clock, "value"),
    1, UINT64_MAX, -1, 0), 1);
  // buffer shorter than the record header
  ck_assert_uint_eq(unserialize_tampered(monikor_metric_integer("m1", &clock, 42, 0),
    1, 1, -1, 0), 0);
  // buffer one byte short of the record
  ck_assert_uint_eq(unserialize_tampered(monikor_metric_integer("m1", &clock, 42, 0),
    1, SERIALIZED_METRIC_HDR_SIZE + 2 + sizeof(uint64_t) - 1, -1, 0), 0);
  // name_size past the end of the buffer
  ck_assert_uint_eq(unserialize_tampered(monikor_metric_integer("m1", &clock, 42, 0),
    1, UINT64_MAX, 65535, 0), 0);
  // string data_size past the end of the buffer
  ck_assert_uint_eq(unserialize_tampered(monikor_metric_string("m1", &clock, "value"),
    1, UINT64_MAX, -1, UINT64_MAX), 0);
  ck_assert_uint_eq(unserialize_tampered(monikor_metric_string("m1", &clock, "value"),
    1, UINT64_MAX, -1, 6), 0);
  // integer data_size that is not 8 bytes
  ck_assert_uint_eq(unserialize_tampered(monikor_metric_integer("m1", &clock, 42, 0),
    1, UINT64_MAX, -1, 4), 0);
  // count larger than the records actually present
  ck_assert_uint_eq(unserialize_tampered(monikor_metric_integer("m1", &clock, 42, 0),
    2, UINT64_MAX, -1, 0), 1);
}
END_TEST


START_TEST(test_monikor_metric_list_unserialize_id) {
  struct timeval clock = { .tv_sec = 42, .tv_usec = 21 };
  void *data = NULL;
  size_t size = 0;
  void *dirty[64];
  monikor_metric_list_t *l;
  monikor_serialized_metric_list_hdr_t hdr;
  monikor_metric_list_t *metrics;
  monikor_metric_t *m;

  l = monikor_metric_list_new();
  ck_assert_ptr_nonnull(l);
  for (int i = 0; i < 32; i++)
    monikor_metric_list_push(l, monikor_metric_integer("m1", &clock, 42, MONIKOR_METRIC_DELTA));
  ck_assert_int_eq(monikor_metric_list_serialize(l, &data, &size), 0);
  monikor_metric_list_free(l);
  monikor_metric_list_header_unserialize(data, &hdr);
  // leave plenty of freed metric-sized chunks full of garbage (past the tcache) to be reused
  for (size_t i = 0; i < sizeof(dirty) / sizeof(*dirty); i++) {
    dirty[i] = malloc(sizeof(monikor_metric_t));
    ck_assert_ptr_nonnull(dirty[i]);
    for (size_t j = 0; j < sizeof(monikor_metric_t); j++)
      ((uint8_t *)dirty[i])[j] = 0xa5;
  }
  for (size_t i = 0; i < sizeof(dirty) / sizeof(*dirty); i++)
    free(dirty[i]);
  ck_assert_uint_eq(monikor_metric_list_unserialize(data + SERIALIZED_METRIC_LIST_HDR_SIZE, &hdr, &metrics), 32);
  while ((m = monikor_metric_list_pop(metrics))) {
    ck_assert_uint_eq(m->id, 0);
    monikor_metric_free(m);
  }

  free(data);
  monikor_metric_list_free(metrics);
}
END_TEST


Suite *libmonikor_metric_list_suite(void) {
  Suite *s = suite_create("libmonikor/metric_list");

  TCase *tc_metric_list_init = tcase_create("monikor_metric_list_init");
  tcase_add_test(tc_metric_list_init, test_monikor_metric_list_init);
  suite_add_tcase(s, tc_metric_list_init);

  TCase *tc_metric_list_new = tcase_create("monikor_metric_list_new");
  tcase_add_test(tc_metric_list_new, test_monikor_metric_list_new);
  suite_add_tcase(s, tc_metric_list_new);

  TCase *tc_metric_list_empty = tcase_create("monikor_metric_list_empty");
  tcase_add_test(tc_metric_list_empty, test_monikor_metric_list_empty);
  suite_add_tcase(s, tc_metric_list_empty);

  TCase *tc_metric_list_push = tcase_create("monikor_metric_list_push");
  tcase_add_test(tc_metric_list_push, test_monikor_metric_list_push);
  suite_add_tcase(s, tc_metric_list_push);

  TCase *tc_metric_list_pop = tcase_create("monikor_metric_list_pop");
  tcase_add_test(tc_metric_list_pop, test_monikor_metric_list_pop);
  suite_add_tcase(s, tc_metric_list_pop);

  TCase *tc_metric_list_apply = tcase_create("monikor_metric_list_apply");
  tcase_add_test(tc_metric_list_apply, test_monikor_metric_list_apply);
  suite_add_tcase(s, tc_metric_list_apply);

  TCase *tc_metric_list_concat = tcase_create("monikor_metric_list_concat");
  tcase_add_test(tc_metric_list_concat, test_monikor_metric_list_concat);
  suite_add_tcase(s, tc_metric_list_concat);

  TCase *tc_metric_list_remove_if = tcase_create("monikor_metric_list_remove_if");
  tcase_add_test(tc_metric_list_remove_if, test_monikor_metric_list_remove_if);
  suite_add_tcase(s, tc_metric_list_remove_if);

  TCase *tc_metric_list_serialize = tcase_create("monikor_metric_list_serialize");
  tcase_add_test(tc_metric_list_serialize, test_monikor_metric_list_serialize);
  tcase_add_test(tc_metric_list_serialize, test_monikor_metric_list_serialize_max_count);
  suite_add_tcase(s, tc_metric_list_serialize);

  TCase *tc_metric_list_unserialize = tcase_create("monikor_metric_list_unserialize");
  tcase_add_test(tc_metric_list_unserialize, test_monikor_metric_list_unserialize);
  tcase_add_test(tc_metric_list_unserialize, test_monikor_metric_list_unserialize_err);
  tcase_add_test(tc_metric_list_unserialize, test_monikor_metric_list_unserialize_id);
  suite_add_tcase(s, tc_metric_list_unserialize);

  return s;
}
