AM_CFLAGS = -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -std=c99 -W -Wall -O2 -I ../../../include/ -I ../../monikor/include
lib_LTLIBRARIES = postgresql.la

postgresql_la_SOURCES = postgresql.c poll.c
postgresql_la_LDFLAGS = -module -avoid-version

postgresqlconfdir = $(sysconfdir)/monikor/modules.d/
postgresqlconf_DATA = ../../../etc/modules/postgresql.yml
