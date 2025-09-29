PROGRAM_NAME := ircserv

COMPILER := c++
STD_VERSION := c++98
FLAGS := -std=$(STD_VERSION)  -Wall -Wextra -Werror

SRC_DIR := src
OBJS_DIR := objs

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJS_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJS_DIR)/%.o)


all: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(OBJS_FILES)
	$(COMPILER) $(FLAGS) $^ -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJS_DIR)
	$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)/*.o

fclean: clean
	rm -rf $(PROGRAM_NAME) $(OBJS_DIR)

re: fclean all

.PHONY: all clean fclean re
