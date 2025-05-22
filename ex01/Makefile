# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/17 12:13:34 by vberdugo          #+#    #+#              #
#    Updated: 2025/05/22 09:17:08 by victor           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Name of the program.
NAME     := PhoneBook

# Sources and objects.
SRCS     := phonebook.cpp
OBJS     := $(SRCS:.cpp=.o)

# Compiler and flags.
CXX      := g++
CXXFLAGS := -Wall -Werror -Wextra -std=c++98
LDFLAGS  := -fsanitize=address,undefined,leak -g

# Default target.
all: $(NAME)

# Link objects into executable.
$(NAME): $(OBJS)
	@$(CXX) $(OBJS) $(LDFLAGS) -o $@ && echo "[INFO] $(NAME) built successfully!"

# Compile sources.
%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean object files.
clean:
	@rm -f $(OBJS)
	@echo "[INFO] Objects removed!"

# Remove all build artifacts.
fclean: clean
	@rm -f $(NAME)
	@echo "[INFO] $(NAME) removed!"

# Rebuild everything.
re: fclean all

# Prevent conflicts with files named clean, all, etc.
.PHONY: all clean fclean re
