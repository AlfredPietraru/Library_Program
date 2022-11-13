FLAGS = -g -lm -Wall -Wextra -std=c99
build: main.c
	gcc -o main main.c $(FLAGS)
clean:
	rm -rf main
	rm -f *.out
run:
	valgrind --leak-check=full --show-leak-kinds=all ./main>results
	