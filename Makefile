CC = gcc
CFLAGS = -Wall -Wextra

SRCS = allocator.c alloc_tests.c
OBJS = $(SRCS:.c=.o)
NAME = alloc_tests

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -f $(OBJS) $(NAME)
