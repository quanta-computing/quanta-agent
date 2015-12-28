# Makefile for Monikor
#
NAME=monikor

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
			modules/load_module.c \
			modules/load_all_modules.c \
			modules/module.c \
			metrics/metric.c \
			metrics/metric_list.c \
			core/init.c \
			core/run.c \
			core/cleanup.c \
			core/usage.c \
			main.c \
			debug.c


SRC=$(addprefix $(SRCD), $(SRCF))
OBJ=$(SRC:.c=.o)

INCD=include/

CC=gcc
CFLAGS=-std=c99 -D_BSD_SOURCE -D_POSIX_SOURCE -W -Wall -O2 -fPIC $(addprefix -I, $(INCD))
LDFLAGS=-rdynamic
LDLIBS=-lyaml -ldl

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ) $(LDLIBS)

all: $(NAME) modules

clean:
	$(RM) $(OBJ)
	$(RM) fork

fclean: clean
	$(RM) $(NAME)

mrproper: fclean

re: mrproper all

cpu:
	make -C ./lib/modules/cpu re

process:
	make -C ./lib/modules/process re

memory:
	make -C ./lib/modules/memory re

apache:
	make -C ./lib/modules/apache re

dummy:
	make -C ./lib/modules/dummy re

modules: dummy cpu process memory apache

fork: ./test/fork.c
	$(CC) $(CFLAGS) -o fork ./test/fork.c

.PHONY: all clean fclean re mrproper cpu dummy modules fork
