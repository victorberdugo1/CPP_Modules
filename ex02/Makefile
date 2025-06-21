# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/21 12:17:13 by vberdugo          #+#    #+#              #
#    Updated: 2025/06/21 12:20:06 by vberdugo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME       := ex02

# Directories
SRC_DIR    := .
OBJ_DIR    := obj

# Source and header files
SRCS       := $(SRC_DIR)/Account.cpp \
              $(SRC_DIR)/tests.cpp
HDRS       := $(SRC_DIR)/Account.hpp

# Object and dependency files
OBJS       := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS       := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.d)

# Compiler and flags
CXX        := g++
CXXFLAGS   := -Wall -Wextra -Werror -std=c++98 -MMD -MP
LDFLAGS    :=

# Default target
all: $(NAME)

# Link objects
$(NAME): $(OBJS)
	@$(CXX) $(OBJS) $(LDFLAGS) -o $@ && echo "[INFO] $(NAME) built successfully!"

# Compile source to object and generate dependencies
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDRS) | $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ && echo "[INFO] Compiled: $<"

# Include dependencies
-include $(DEPS)

# Create object directory
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Run tests
.PHONY: test
test: $(NAME)
	@./$(NAME) | tee run.log

# Clean objects
clean:
	@rm -rf $(OBJ_DIR)
	@echo "[INFO] Object and dependency files removed!"

# Clean executable and logs
fclean: clean
	@rm -f $(NAME) run.log
	@echo "[INFO] Executable and logs removed!"

# Full rebuild
re: fclean all

.PHONY: all clean fclean re test
