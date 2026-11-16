ish: main.c input.c builtins.c exec.c vars.c pipe.c redirection.c prompt.c ish.h
	gcc -o ish main.c input.c builtins.c exec.c vars.c pipe.c redirection.c prompt.c -Wall