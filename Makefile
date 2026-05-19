NAME = codexion
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread -Iinc -g
SRC_DIR = src
OBJ_DIR = obj
SRC = parsing.c main.c coder_utils.c coder.c monitor_utils.c monitor_check.c monitor_stop.c monitor.c dongle_utils.c dongle.c queue_fifo.c queue_edf.c utils.c sim_init.c sim_threads.c sim_cleanup.c
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re