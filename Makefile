NAME = traceroute

CC_FLAGS = -fsanitize=address -g3 -Wall -Wextra -Werror -I includes/

SRC_DIR = src/

SRC = $(SRC_DIR)main.c \
			$(SRC_DIR)parser.c \
			$(SRC_DIR)client.c \
			$(SRC_DIR)builder.c \
			$(SRC_DIR)main_loop.c \
			$(SRC_DIR)hop_loop.c \
			$(SRC_DIR)recv_loop.c \
			$(SRC_DIR)sender.c \
			$(SRC_DIR)print_start.c \
			$(SRC_DIR)exit.c

OBJ_DIR = obj/

OBJ = $(patsubst $(SRC_DIR)%.c, $(OBJ_DIR)%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	@echo "Linking..."
	@gcc $(CC_FLAGS) -o $@ $^ -lm
	@echo "$(NAME) compiled successfully!"

$(OBJ_DIR):
	@mkdir -p $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	@gcc $(CC_FLAGS) -c $< -o $@
	@echo "$< compiled successfully!"

clean:
	@echo "Cleaning object files..."
	@rm -rf $(OBJ_DIR)
	@echo "Object files cleaned."

fclean: clean
	@echo "Removing executable..."
	@rm -f $(NAME)
	@echo "Executable removed."

re: fclean all