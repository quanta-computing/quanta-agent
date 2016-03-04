AM_CFLAGS = -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -std=c99 -W -Wall -O2 -I ../../../include/ -I ../../monikor/include
lib_LTLIBRARIES = varnish.la

varnish_la_SOURCES = varnish.c poll.c
varnish_la_LDFLAGS = -module -avoid-version
varnish_la_LIBADD = -lvarnishapi -ldl

varnishconfdir = $(sysconfdir)/monikor/modules.d/
varnishconf_DATA = ../../../etc/modules/varnish.yml
