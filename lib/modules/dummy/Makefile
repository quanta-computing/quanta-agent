NAME=../dummy.dylib
SRC=dummy.c
OBJ=$(SRC:.c=.o)

CC=gcc
CFLAGS=-I ../../../include/ -fPIC -fno-common -W -Wall
LDFLAGS=-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0

RM=@rm -vf


$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ)

all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
