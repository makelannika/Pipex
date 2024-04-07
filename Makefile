# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: amakela <amakela@student.hive.fi>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/24 16:11:50 by amakela           #+#    #+#              #
#    Updated: 2024/03/24 16:27:47 by amakela          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	pipex

LIBFTDIR	=	libft
LIBFT		=	$(LIBFTDIR)/libft.a

CFLAGS		=	-Wall -Wextra -Werror
CC			=	cc

CFILES		=	pipex.c

OFILES		=	$(CFILES:.c=.o)

all: $(LIBFT) $(NAME)

$(LIBFT):
	make -C $(LIBFTDIR)

$(NAME): $(OFILES)
	$(CC) $(OFILES) $(LIBFT) -o $(NAME)

clean:
	rm -f $(OFILES)
	make -C $(LIBFTDIR) clean

fclean: clean
	rm -f $(NAME)
	make -C $(LIBFTDIR) fclean

re: fclean all

.PHONY: all clean fclean re
