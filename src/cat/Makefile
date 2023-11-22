GCC = gcc -Wall -Wextra -Werror
PATH1 = ./s21_cat

all: s21_cat
clean: 
	rm -rf *.out *.o *.log $(PATH1)
rebuild: clean all

s21_cat: s21_cat.c
	$(GCC) $^ -o $(PATH1) 
