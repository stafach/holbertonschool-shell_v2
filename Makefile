NAME = hsh

CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

SRC = main.c input.c env.c env2.c strings.c parse.c path.c execute.c builtins.c cd.c redirect.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c hsh.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
