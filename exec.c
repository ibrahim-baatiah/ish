#include "ish.h"

int ish_launch(char **args) {
    pid_t rc = fork();

    if (rc < 0) { // Fork Failed
        fprintf(stderr, "Fork failed\n");
        exit(1);
    } else if (rc == 0) { // This is the child
        if (handle_redirect(args) == -1) {
            fprintf(stderr, "Couldn't redirect\n");
            exit(1);
        }
        if (execvp(args[0], args) == -1) {
            perror("ish");
        }
        _exit(EXIT_FAILURE);
    } else { // This is the parent
        int status;
        do {
            waitpid(rc, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int ish_execute(char **args) {
    char *eq;
    for (int i = 0; args[i] != NULL; i++) {
        args[i] = ish_expand_token(args[i]);
    }
    if (strcmp(args[0], "cd") == 0) return ish_cd(args);
    else if (strcmp(args[0], "exit") == 0) return ish_exit(args);
    else if (strcmp(args[0], "help") == 0) return ish_help(args);
    else if ((eq = strchr(args[0], '=')) != NULL) return ish_is_assignment(args[0], eq);
    else ish_launch(args);
    return 1;
}