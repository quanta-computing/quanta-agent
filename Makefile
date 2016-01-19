# Makefile for Monikor
#
NAME=monikor

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
	LIBMONIKOR=lib/monikor/libmonikor.dylib
else
	LIBMONIKOR=lib/monikor/libmonikor.so
endif

SRCD=src/
SRCF=	config/config.c \
			config/setup_config.c \
			config/config_dict.c \
			config/config_parser.c \
			config/config_parse_key.c \
			config/config_parse_mapping.c \
			config/config_parse_list.c \
			utils/strl.c \
			utils/logger.c \
			utils/read_file.c \
			utils/net.c \
			utils/time.c \
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
			core/exit.c \
			core/init.c \
			core/run.c \
			core/send.c \
			core/sig.c \
			core/usage.c \
			main.c \
			debug.c


SRC=$(addprefix $(SRCD), $(SRCF))
OBJ=$(SRC:.c=.o)

INCD=include/ lib/monikor/include/

CC=gcc
ifeq ($(UNAME),Darwin)
	CFLAGS=-std=c99 -D_BSD_SOURCE -W -Wall -O2 -fPIC $(addprefix -I, $(INCD))
else
	# CFLAGS=-std=c99 -D_GNU_SOURCE -D_POSIX_SOURCE -W -Wall -O2 -fPIC $(addprefix -I, $(INCD))
	CFLAGS=-std=c99 -D_GNU_SOURCE -D_POSIX_SOURCE -W -Wall -g -g3 -fPIC $(addprefix -I, $(INCD))
endif
LDFLAGS=-rdynamic -Llib/monikor
LDLIBS=-lyaml -lcurl -ldl -lmonikor

$(NAME): $(OBJ) $(LIBMONIKOR)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ) $(LDLIBS)

all: $(NAME)

clean:
	$(RM) $(OBJ)
	$(RM) fork

fclean: clean
	$(RM) $(NAME)
	make -C ./lib/monikor fclean

mrproper: fclean

re: mrproper all

$(LIBMONIKOR):
	make -C ./lib/monikor/

cpu:
	make -C ./lib/modules/cpu re

process:
	make -C ./lib/modules/process re

memory:
	make -C ./lib/modules/memory re

network:
	make -C ./lib/modules/network re

apache:
	make -C ./lib/modules/apache re

dummy:
	make -C ./lib/modules/dummy re

modules: dummy cpu process memory apache network

fork: ./test/fork.c
	$(CC) $(CFLAGS) -o fork ./test/fork.c

client: ./test/client.c
	$(CC) $(CFLAGS) -o client ./test/client.c -Llib/monikor -lmonikor

.PHONY: all clean fclean mrproper re cpu dummy network modules fork client
