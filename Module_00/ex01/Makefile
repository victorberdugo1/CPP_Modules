# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/17 12:13:34 by vberdugo          #+#    #+#              #
#    Updated: 2025/06/03 13:42:36 by vberdugo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME       := crappyPhonebook

# Directories
OBJ_DIR    := obj
SRC_DIR    := .

# Source files
SRCS       := $(SRC_DIR)/crappyPhonebook.cpp \
              $(SRC_DIR)/PhoneBook.class.cpp \
              $(SRC_DIR)/Contact.class.cpp

# Object and dependency files (in obj/)
OBJS       := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS       := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.d)

# Compiler and flags
CC         := g++
CFLAGS     := -Wall -Wextra -Werror -std=c++98 -MMD -MP
LDFLAGS    := -fsanitize=address,undefined,leak -g

# Default target
all: $(NAME)

# Link object files into executable
$(NAME): $(OBJS)
	@$(CC) $(OBJS) $(LDFLAGS) -o $@ && echo "[INFO] $(NAME) built successfully!"

# Compile .cpp into .o and generate .d dependency file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@ && echo "[INFO] Compiled: $<"

# Include dependency files (auto-regenerate on header changes)
-include $(DEPS)

# Create object directory if it doesn't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Remove object and dependency files
clean:
	@rm -rf $(OBJ_DIR)
	@echo "[INFO] Object and dependency files removed!"

# Remove everything including executable
fclean: clean
	@rm -f $(NAME)
	@echo "[INFO] $(NAME) removed!"

# Full rebuild
re: fclean all

.PHONY: all clean fclean re
