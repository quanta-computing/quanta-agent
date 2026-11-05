TESTS = check_libmonikor check_varnish
check_PROGRAMS = check_libmonikor check_varnish
noinst_PROGRAMS = varnishstat-parse

check_libmonikor_SOURCES = libmonikor/check.c \
  libmonikor/metric.c \
  libmonikor/metric_list.c \
  libmonikor/metric_store.c
check_libmonikor_CFLAGS = @CHECK_CFLAGS@ `pkg-config --cflags --libs check`
check_libmonikor_LDADD = $(top_builddir)/lib/monikor/src/libmonikor.la

# The varnish module's parser is compiled into the tests with stand-ins for the agent core
# (varnish/stubs.c), and fed saved `varnishstat -1` outputs from varnish/fixtures.
VARNISH_MODULE_CFLAGS = -D_DEFAULT_SOURCE -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L -std=c99 -W -Wall \
  -I$(top_srcdir)/include -I$(top_srcdir)/lib/monikor/include -I$(top_srcdir)/lib/modules/varnish \
  -I$(srcdir)/varnish
VARNISH_MODULE_SOURCES = varnish/stubs.c $(top_srcdir)/lib/modules/varnish/poll.c

check_varnish_SOURCES = varnish/check.c $(VARNISH_MODULE_SOURCES)
check_varnish_CFLAGS = @CHECK_CFLAGS@ $(VARNISH_MODULE_CFLAGS) \
  -DVARNISH_FIXTURES_DIR='"$(srcdir)/varnish/fixtures"'
check_varnish_LDADD = $(top_builddir)/lib/monikor/src/libmonikor.la @CHECK_LIBS@

# varnishstat-parse <file>: prints the metrics the module extracts from a varnishstat -1 capture
varnishstat_parse_SOURCES = varnish/varnishstat_parse.c $(VARNISH_MODULE_SOURCES)
varnishstat_parse_CFLAGS = $(VARNISH_MODULE_CFLAGS)
varnishstat_parse_LDADD = $(top_builddir)/lib/monikor/src/libmonikor.la

EXTRA_DIST = varnish/stubs.h varnish/fixtures
