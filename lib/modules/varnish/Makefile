UNAME=$(shell uname)

ifeq ($(UNAME),Darwin)
	LDFLAGS=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0
	DYLIB_SUFFIX=dylib
else
	DYLIB_SUFFIX=so
	LDFLAGS=-shared -Wl,--export-dynamic
endif

NAME=../varnish.$(DYLIB_SUFFIX)
SRC=varnish.c \
	poll.c
OBJ=$(SRC:.c=.o)

CC=gcc
ifeq ($(UNAME),Darwin)
	CFLAGS=-std=c99 -O2 -D_BSD_SOURCE -I ../../../include/ -I ../../monikor/include -fPIC -fno-common -W -Wall
else
	CFLAGS=-std=c99 -g -g3 -D_GNU_SOURCE -D_POSIX_SOURCE -I ../../../include/ -I ../../monikor/include -fPIC -fno-common -W -Wall
	# CFLAGS=-std=c99 -O2 -D_GNU_SOURCE -D_POSIX_SOURCE -I ../../../include/ -I ../../monikor/include -fPIC -fno-common -W -Wall
endif

RM=@rm -vf

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ) -lvarnishapi -ldl

all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
