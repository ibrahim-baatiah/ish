#ifndef ISH_H
#define ISH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

void ish_loop(FILE *stream, int interactive);
char *ish_read_line(FILE *stream);
char **ish_split_line(char *line);
int ish_launch(char **args);
int ish_cd(char **args);
int ish_exit(char **args);
int ish_help(char **args);
int ish_execute(char **args);
int handle_redirect(char **args);
int ish_is_assignment(char *token, char *eq);
int ish_set_var(char *name, char *value);
char *ish_expand_token(char *token);
int ish_pipe_split(char **args);

#endif