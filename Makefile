SRC = pipex.c \
	  pipex_utils.c \
	  pipex_utils2.c \
	  ft_split.c \
	  tools_libft.c \
	  ft_strncmp.c \

NAME = pipex
OBJ = $(SRC:.c=.o)
CC = cc
CFLAGS = -Wall -Wextra -Werror
OUTFILE = outfile
all: $(NAME)
$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
clean:
	rm -f $(OBJ)
	rm -f $(OUTFILE)
fclean: clean
	rm -f $(NAME)
re: fclean all
.PHONY: clean fclean all re
