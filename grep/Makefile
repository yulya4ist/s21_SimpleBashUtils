FILES = func.c s21_grep.c
FLAGS = -Wall -Werror -Wextra

all: 
	gcc $(FLAGS) $(FILES) -o s21_grep

s21_grep:
	gcc $(FLAGS) $(FILES) -o s21_grep

clean:
	rm -rf *.out s21_grep *.dSYM *.o

rebuild: clean all