CC=gcc
NAME=main
DEPS=main.c

compile: $(DEPS) 
	$(CC) $(DEPS) -o $(NAME)

run:
	./$(NAME)

clean:
	rm $(NAME)

all: compile
