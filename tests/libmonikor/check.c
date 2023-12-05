#include <check.h>

Suite *libmonikor_metric_suite(void);
Suite *libmonikor_metric_list_suite(void);
Suite *libmonikor_metric_store_suite(void);


int main(void) {
  int failed;
  Suite *metric_suite = libmonikor_metric_suite();
  SRunner *runner = srunner_create(metric_suite);

  srunner_add_suite(runner, libmonikor_metric_list_suite());
  srunner_add_suite(runner, libmonikor_metric_store_suite());
  srunner_run_all(runner, CK_NORMAL);
  failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return !(failed == 0);
}
