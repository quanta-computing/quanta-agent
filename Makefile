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
CFLAGS=-W -Wall -O2 -fPIC $(addprefix -I, $(INCD))
LDFLAGS=
LDLIBS=-lyaml

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ) $(LDLIBS)

all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

mrproper: fclean

re: mrproper all

.PHONY: all clean fclean re mrproper
