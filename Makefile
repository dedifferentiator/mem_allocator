CC=gcc
NAME=main
DEPS=main.c block.c
LIBS=block.h

compile: $(DEPS) $(LIBS)
	$(CC) $(DEPS) -o $(NAME)

run:
	./$(NAME)

clean:
	rm $(NAME)

all: compile
