AM_CFLAGS=-std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L -W -Wall -fPIC -I../include -I../lib/monikor/include
AM_LDFLAGS=-rdynamic
bin_PROGRAMS = monikor
noinst_LTLIBRARIES = libmonikorexe.la

monikorconfdir=$(sysconfdir)/monikor
monikorconf_DATA=../etc/config.yml

monikor_LDADD = $(top_builddir)/lib/monikor/src/libmonikor.la libmonikorexe.la
monikor_LDFLAGS = -Wl,--export-dynamic -Wl,--whole-archive,./.libs/libmonikorexe.a,--no-whole-archive
monikor_SOURCES = main.c

libmonikorexe_la_LIBADD = $(top_builddir)/lib/monikor/src/libmonikor.la -lyaml -lcurl -ldl -lm
libmonikorexe_la_SOURCES = config/config.c \
			config/setup_config.c \
			config/config_dict.c \
			config/config_parser.c \
			config/config_parse_key.c \
			config/config_parse_mapping.c \
			config/config_parse_list.c \
			utils/command.c \
			utils/http.c \
			utils/logger.c \
			utils/net.c \
			utils/read_file.c \
			utils/strl.c \
			modules/info_module.c \
			modules/load_module.c \
			modules/load_all_modules.c \
			modules/module.c \
			io_handler/io_handler.c \
			io_handler/list.c \
			io_handler/monikor.c \
			io_handler/poll.c \
			server/cleanup.c \
			server/disconnect.c \
			server/handle.c \
			server/init.c \
			core/cleanup.c \
			core/control.c \
			core/daemonize.c \
			core/evict.c \
			core/exit.c \
			core/init.c \
			core/metrics.c \
			core/poll.c \
			core/reload.c \
			core/run.c \
			core/send.c \
			core/sig.c \
			core/update.c \
			core/usage.c \
			debug.c
