UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
	LDFLAGS=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0
	DYLIB_SUFFIX=dylib
else
	DYLIB_SUFFIX=so
	LDFLAGS=-shared -Wl,--export-dynamic
endif

NAME=../process.$(DYLIB_SUFFIX)
SRC=process.c \
	poll.c
OBJ=$(SRC:.c=.o)

CC=gcc
CFLAGS=-std=c99 -D_BSD_SOURCE -I ../../../include/ -I ../../monikor/include -fPIC -fno-common -W -Wall

RM=@rm -vf

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ)

all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
