AM_CFLAGS = -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -std=c99 -W -Wall -O2 -I ../../../include/ -I /usr/include/varnish -I ../../monikor/include
lib_LTLIBRARIES = varnish4.la

varnish4_la_SOURCES = varnish4.c poll.c
varnish4_la_LDFLAGS = -module -avoid-version
varnish4_la_LIBADD = -lvarnishapi -ldl

varnish4confdir = $(sysconfdir)/monikor/modules.d/
varnish4conf_DATA = ../../../etc/modules/varnish4.yml
