#include "ish.h"

int ish_pipe(int pipe_arr[][2], char ***cmds, int length);

int ish_pipe_split(char **args) {
    int capacity = 16;
    int length = 0;
    char ***cmds = malloc(capacity * sizeof(char **));

    cmds[length++] = &args[0];
    for (int i = 0; args[i] != NULL; i++) {
        if (length + 1 >= capacity) {
            capacity *= 2;
            char ***grown = realloc(cmds, capacity * sizeof(char **));
            if (grown == NULL) {
                free(cmds);
                exit(1);
            }
            cmds = grown;
        }
        if ((strcmp(args[i], "|") == 0)) {
            if (args[i+1] == NULL) {
                fprintf(stderr, "ish: expected command after |\n");
                free(cmds);
                return 1;
            }
            args[i] = NULL;
            cmds[length++] = &args[i+1];
        }
    }
    if (length < 2) {
        free(cmds);
        return 0;
    }
    int pipe_arr[length - 1][2];
    for (int i = 0; i < length - 1; i++) {
        if (pipe(pipe_arr[i]) == -1) {
            perror("ish");
            free(cmds);
            return -1;
        }
    }
    int res = ish_pipe(pipe_arr, cmds, length);
    free(cmds);
    return res;
}

int ish_pipe(int pipe_arr[][2], char ***cmds, int length) {
    pid_t pids[length];

    for (int i = 0; i < length; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("ish: fork");
            // don't continue trying to build a broken pipeline
            for (int j = 0; j < i; j++) kill(pids[j], SIGTERM);  // clean up already-started children
            return 1;
        }

        if (pids[i] == 0) {
            // The child
            if (i > 0) {
                // Read from left
                dup2(pipe_arr[i - 1][0], STDIN_FILENO);
            }
            if (i < length - 1) {
                // Write to right
                dup2(pipe_arr[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < length - 1; j++) {
                close(pipe_arr[j][0]);
                close(pipe_arr[j][1]);
            }

            if (execvp(cmds[i][0], cmds[i]) == -1) {
                perror("ish: pipe opearion failed!\n");
            }
            _exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < length - 1; i++) {
            close(pipe_arr[i][0]);
            close(pipe_arr[i][1]);        
    }

    for (int i = 0; i < length; i++) {
        waitpid(pids[i], NULL, 0);
    }
    return 1;
}