TESTS = check_libmonikor
check_PROGRAMS = check_libmonikor
check_libmonikor_SOURCES = libmonikor/check.c \
  libmonikor/metric.c \
  libmonikor/metric_list.c \
  libmonikor/metric_store.c
check_libmonikor_CFLAGS = @CHECK_CFLAGS@ `pkg-config --cflags --libs check`
check_libmonikor_LDADD = $(top_builddir)/lib/monikor/src/libmonikor.la
