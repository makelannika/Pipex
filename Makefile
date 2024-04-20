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

BONUS		=	pipex_bonus

LIBFTDIR	=	libft
LIBFT		=	$(LIBFTDIR)/libft.a

CFLAGS		=	-Wall -Wextra -Werror
CC			=	cc -g

MAINC		=	pipex.c
MAINO		=	$(MAINC:.c=.o)

CFILES		=	init_data.c		fd_utils.c		forking.c	\
				arg_parsing.c	utils.c

MAINCB		=	pipex_bonus.c
MAINOB		=	$(MAINCB:.c=.o)

OFILES		=	$(CFILES:.c=.o)

all: $(NAME)

$(NAME): $(OFILES) $(MAINO) $(LIBFT) 
	$(CC) $(CFLAGS) $(OFILES) $(MAINO) $(LIBFT) -o $(NAME)

bonus: $(BONUS)

$(BONUS): $(OFILES) $(MAINOB) $(LIBFT)
	$(CC) $(CFLAGS) $(OFILES) $(MAINOB) $(LIBFT) -o $(BONUS)

$(LIBFT):
	make -C $(LIBFTDIR)

clean:
	rm -f $(OFILES) $(MAINO) $(MAINOB)
	make -C $(LIBFTDIR) clean

fclean: clean
	rm -f $(NAME) pipex_bonus
	make -C $(LIBFTDIR) fclean

re: fclean all

.PHONY: all clean fclean re
