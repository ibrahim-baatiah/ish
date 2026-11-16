#include "ish.h"

int handle_redirect(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "ish: expected filename after >\n");
                return -1;
            }
            if (freopen(args[i+1], "w", stdout) == NULL) return -1;
            // Terminate after >
            args[i] = NULL;
        } else if (strcmp(args[i], "<") == 0) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "ish: expected filename after <\n");
                return -1;
            }
            if (freopen(args[i+1], "r", stdin) == NULL) return -1;
            args[i] = NULL;
        }
    }
    return 0;
}