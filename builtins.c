#include "ish.h"

int ish_cd(char **args) {
    if (args[1] == NULL) fprintf(stderr, "No path was provided\n");
    else {
        if (chdir(args[1]) == -1) {
        fprintf(stderr, "cd to %s failed: %s\n", args[1], strerror(errno));
        }
    }
    return 1;
}

int ish_exit(char **args) {
    return 0;
}

int ish_help(char **args) {
    printf("This is my help page, I'll improve it later\n");
    return 1;
}