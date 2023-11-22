GCC = gcc -Wall -Wextra -Werror
PATH1 = ./s21_grep

all: s21_grep
clean: 
	rm -rf *.out *.o *.log $(PATH1)
rebuild: clean all

s21_grep: s21_grep.c
	$(GCC) $^ -o $(PATH1) 