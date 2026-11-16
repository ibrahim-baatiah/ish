#include <stdio.h>
#include "ish.h"

int main(int argc, char **argv) {
    int interactive = 1;
    if (argc > 1) {
        // First arg is the name of the script file
        if (freopen(argv[1], "r", stdin) == NULL) {
            fprintf(stderr, "Can't read script from file %s\n", argv[1]);
            fprintf(stderr, "Exiting\n");
            exit(1);
        }
        interactive = 0;
    }
    ish_loop(stdin, interactive);
    return EXIT_SUCCESS;
}

void ish_loop(FILE *stream, int interactive) {
    int state = 1;
    ish_prompt_init();
    do {
        if (interactive) {
            char *prompt = ish_render_prompt();
            fputs(prompt, stdout);
            fflush(stdout);
            free(prompt);
        }
        char *line = ish_read_line(stream);
        if (line == NULL) {
            state = 0;
        } else {
            char **args = ish_split_line(line);
            if (args[0] != NULL) {
                // state = ish_execute(args);
                if ((state = ish_pipe_split(args)) == 0) {
                     state = ish_execute(args);
                }
                ish_prompt_next_command();
            }
            free(line);
            free(args);   
        }
        
    } while (state != 0); 
}