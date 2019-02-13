AM_CFLAGS = -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -std=c99 -W -Wall -O2 -I ../../../include/ -I ../../monikor/include
lib_LTLIBRARIES = postfix.la

postfix_la_SOURCES = postfix.c poll.c
postfix_la_LDFLAGS = -module -avoid-version

postfixconfdir = $(sysconfdir)/monikor/modules.d/
postfixconf_DATA = ../../../etc/modules/postfix.yml
