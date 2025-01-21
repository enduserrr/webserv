NAME	= webserv
SRC		= main.cpp
OBJ_DIR	= .obj
OBJ		= $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
CC		= c++
RM		= rm -fr
FLAGS	= -Wall -Wextra -Werror -std=c++11

WHITE_B	= \33[1;97m
RESET 	= \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "$(WHITE_B)BUILT SUCCESFULLY$(RESET)"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: re
	clear
	./$(NAME)

.PHONY: all clean fclean re
