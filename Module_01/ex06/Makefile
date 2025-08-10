# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/10 12:11:23 by vberdugo          #+#    #+#              #
#    Updated: 2025/08/10 12:28:58 by vberdugo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC = main.cpp Harl.cpp
OBJ = $(SRC:.cpp=.o)
CC = g++
RM = rm -f

UNAME = $(shell uname -s)
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic

ifeq ($(UNAME), Linux)
CPPFLAGS = -Wall -Wextra -std=c++98 -pedantic
$(warning "Linux detected! Disabling -Werror...")
endif

NAME = harlFilter

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re
