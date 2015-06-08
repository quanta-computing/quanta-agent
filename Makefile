# Makefile for Monikor
#
NAME=monikor

SRCD=src/
SRCF=	config/config.c \
			config/config_dict.c \
			config/config_parser.c \
			config/config_parse_key.c \
			config/config_parse_mapping.c \
			debug.c \
			load_module.c \
			logger.c \
			main.c \
			metric.c \
			metric_list.c \
			module.c
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
