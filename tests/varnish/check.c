/*
** Varnish module parser tests. The fixtures under fixtures/ are `varnishstat -1` outputs (one per
** Varnish generation: 4.1, 6.0, 7.6) with the metrics the module must extract from each listed
** in the matching .expected file (`name value` per line). Every expected metric must come out
** with that value and nothing else must come out.
*/
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "varnish.h"
#include "stubs.h"

#ifndef VARNISH_FIXTURES_DIR
# define VARNISH_FIXTURES_DIR "varnish/fixtures"
#endif

static char *read_file(const char *name) {
  char path[1024];
  FILE *f;
  long size;
  char *buf;

  snprintf(path, sizeof(path), "%s/%s", VARNISH_FIXTURES_DIR, name);
  ck_assert_msg((f = fopen(path, "r")) != NULL, "cannot open fixture %s", path);
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  rewind(f);
  buf = malloc(size + 1);
  ck_assert_ptr_nonnull(buf);
  ck_assert_int_eq(fread(buf, 1, size, f), size);
  buf[size] = 0;
  fclose(f);
  return buf;
}

static size_t parse(const char *output, char *instance) {
  varnish_module_t mod = { .instance = instance, .mon = NULL, .cmd_handler = NULL };

  recorded_metrics_reset();
  return varnish_fetch_metrics(&mod, output);
}

static void check_fixture(const char *fixture) {
  char output_name[256], expected_name[256];
  char *output, *expected, *line, *saveptr;
  size_t n_expected = 0, n_metrics;

  snprintf(output_name, sizeof(output_name), "%s.txt", fixture);
  snprintf(expected_name, sizeof(expected_name), "%s.expected", fixture);
  output = read_file(output_name);
  expected = read_file(expected_name);
  n_metrics = parse(output, NULL);
  for (line = strtok_r(expected, "\n", &saveptr); line; line = strtok_r(NULL, "\n", &saveptr)) {
    char name[256];
    unsigned long long value;
    const recorded_metric_t *rec;

    ck_assert_msg(sscanf(line, "%255s %llu", name, &value) == 2, "%s: bad expected line '%s'", fixture, line);
    n_expected++;
    rec = recorded_metric_find(name);
    ck_assert_msg(rec != NULL, "%s: metric %s not extracted", fixture, name);
    ck_assert_msg(rec->value == value, "%s: metric %s = %llu, expected %llu",
      fixture, name, (unsigned long long)rec->value, value);
  }
  ck_assert_msg(n_metrics == n_expected, "%s: %zu metrics extracted, %zu expected",
    fixture, n_metrics, n_expected);
  ck_assert_uint_eq(recorded_metrics_count, n_expected);
  free(output);
  free(expected);
}

START_TEST(test_varnish_4_1) {
  check_fixture("varnishstat-4.1");
}
END_TEST

START_TEST(test_varnish_6_0) {
  check_fixture("varnishstat-6.0");
}
END_TEST

START_TEST(test_varnish_7_6) {
  check_fixture("varnishstat-7.6");
}
END_TEST

START_TEST(test_instance_suffix) {
  char *output = read_file("varnishstat-6.0.txt");
  const recorded_metric_t *rec;

  parse(output, "front1");
  ck_assert_ptr_null(recorded_metric_find("varnish.requests_per_second"));
  rec = recorded_metric_find("varnish.requests_per_second.front1");
  ck_assert_ptr_nonnull(rec);
  ck_assert_uint_eq(rec->flags, MONIKOR_METRIC_TIMEDELTA);
  free(output);
}
END_TEST

// Regression: a plain substring search resolved client_req to client_req_400, printed first
START_TEST(test_counter_is_matched_as_a_whole_token) {
  const char *output =
    "MAIN.client_req_400             5         0.00 Client requests received, subject to 400 errors\n"
    "MAIN.client_req_417             7         0.00 Client requests received, subject to 417 errors\n"
    "MAIN.client_req            123456        12.34 Good client requests received\n";
  const recorded_metric_t *rec;

  ck_assert_uint_eq(parse(output, NULL), 1);
  rec = recorded_metric_find("varnish.requests_per_second");
  ck_assert_ptr_nonnull(rec);
  ck_assert_uint_eq(rec->value, 123456);
}
END_TEST

// Regression: the value was read at an offset derived from the Quanta metric name length
START_TEST(test_value_is_read_right_after_the_counter_name) {
  const char *output = "MAIN.backend_unhealthy 98765 1.00 Backend conn. not attempted\n";
  const recorded_metric_t *rec;

  ck_assert_uint_eq(parse(output, NULL), 1);
  rec = recorded_metric_find("varnish.backends.connections.unhealthy");
  ck_assert_ptr_nonnull(rec);
  ck_assert_uint_eq(rec->value, 98765);
}
END_TEST

START_TEST(test_counter_with_a_suffix_does_not_match) {
  // Varnish 7 only has sess_dropped, not sess_drop
  const char *output = "MAIN.sess_dropped              42         0.00 Sessions dropped for thread\n";

  ck_assert_uint_eq(parse(output, NULL), 0);
}
END_TEST

START_TEST(test_counter_word_in_a_description_does_not_match) {
  const char *output =
    "MAIN.thread_queue_len           0          .   Length of session queue waiting for threads\n"
    "MAIN.pools                      2          .   Number of thread pools\n";

  ck_assert_uint_eq(parse(output, NULL), 0);
}
END_TEST

START_TEST(test_uptime_is_the_child_process_one) {
  const char *output =
    "MGT.uptime                    999         1.00 Management process uptime\n"
    "MAIN.uptime                   500         1.00 Child process uptime\n";
  const recorded_metric_t *rec;

  ck_assert_uint_eq(parse(output, NULL), 1);
  rec = recorded_metric_find("varnish.uptime");
  ck_assert_ptr_nonnull(rec);
  ck_assert_uint_eq(rec->value, 500);
}
END_TEST

START_TEST(test_transient_storage_is_skipped_whatever_its_position) {
  const char *output =
    "SMA.Transient.g_bytes          11          .   Bytes outstanding\n"
    "SMA.s0.g_bytes                 22          .   Bytes outstanding\n"
    "SMA.s0.g_space                 33          .   Bytes available\n"
    "SMA.Transient.g_space          44          .   Bytes available\n";

  ck_assert_uint_eq(parse(output, NULL), 2);
  ck_assert_uint_eq(recorded_metric_find("varnish.memory.used")->value, 22);
  ck_assert_uint_eq(recorded_metric_find("varnish.memory.free")->value, 33);
}
END_TEST

START_TEST(test_empty_output) {
  ck_assert_uint_eq(parse("", NULL), 0);
}
END_TEST

static Suite *varnish_suite(void) {
  Suite *s = suite_create("varnish");
  TCase *fixtures = tcase_create("fixtures");
  TCase *matching = tcase_create("matching");

  tcase_add_test(fixtures, test_varnish_4_1);
  tcase_add_test(fixtures, test_varnish_6_0);
  tcase_add_test(fixtures, test_varnish_7_6);
  tcase_add_test(fixtures, test_instance_suffix);
  suite_add_tcase(s, fixtures);
  tcase_add_test(matching, test_counter_is_matched_as_a_whole_token);
  tcase_add_test(matching, test_value_is_read_right_after_the_counter_name);
  tcase_add_test(matching, test_counter_with_a_suffix_does_not_match);
  tcase_add_test(matching, test_counter_word_in_a_description_does_not_match);
  tcase_add_test(matching, test_uptime_is_the_child_process_one);
  tcase_add_test(matching, test_transient_storage_is_skipped_whatever_its_position);
  tcase_add_test(matching, test_empty_output);
  suite_add_tcase(s, matching);
  return s;
}

int main(void) {
  int failed;
  SRunner *runner = srunner_create(varnish_suite());

  srunner_run_all(runner, CK_NORMAL);
  failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return !(failed == 0);
}
