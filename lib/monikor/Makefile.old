# Makefile for libmonikor
# Should work on linux and OSX
UNAME=$(shell uname)

ifeq ($(UNAME),Darwin)
	DYLIB_SUFFIX=dylib
else
	DYLIB_SUFFIX=so
endif

NAME=libmonikor.$(DYLIB_SUFFIX)

SRCD=src/
SRCF= metrics/metric.c \
			metrics/metric_list.c \
			metrics/metric_store.c \
			metrics/metric_compute.c \
			metrics/metric_list_serialize.c \
			metrics/metric_list_unserialize.c \
			utils/serialize.c

SRC=$(addprefix $(SRCD), $(SRCF))
OBJ=$(SRC:.c=.o)
INCD=include/

INCD=include/

CC=gcc
CFLAGS=-std=c99 -D_GNU_SOURCE -W -Wall -g -g3 -fPIC $(addprefix -I, $(INCD))
ifeq ($(UNAME),Darwin)
	# CFLAGS=-std=c99 -D_GNU_SOURCE -W -Wall -O2 -fPIC $(addprefix -I, $(INCD))
	LDFLAGS=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0
else
	LDFLAGS=-shared -Wl,--export-dynamic
endif

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ)


all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
