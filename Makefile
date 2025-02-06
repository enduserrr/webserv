NAME	= webserv
SRC_DIR = src
SRC		= $(SRC_DIR)/main.cpp $(SRC_DIR)/ConfParser.cpp \
		  $(SRC_DIR)/ServerBlock.cpp $(SRC_DIR)/Location.cpp \
		  $(SRC_DIR)/ServerLoop.cpp $(SRC_DIR)/HttpParser.cpp \
		  $(SRC_DIR)/ErrorHandler.cpp $(SRC_DIR)/StaticHandler.cpp \
		  $(SRC_DIR)/CgiHandler.cpp $(SRC_DIR)/UploadHandler.cpp \
		  $(SRC_DIR)/HttpRequest.cpp $(SRC_DIR)/Router.cpp \
		  $(SRC_DIR)/Methods.cpp
OBJ_DIR	= .obj
OBJ		= $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.cpp=.o)))
CC		= c++
RM		= rm -fr
FLAGS	= -Wall -Wextra -Werror -std=c++11 -I incs
FSANITIZE = -g3 -fsanitize=address

WHITE_B	= \33[1;97m
RESET 	= \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(FSANITIZE) $(OBJ) -o $(NAME)
	@echo "$(WHITE_B)BUILT SUCCESFULLY$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(FLAGS) $(FSANITIZE) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: re
	clear
	./$(NAME) config/config3.conf

.PHONY: all clean fclean re run
