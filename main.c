#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

void ish_loop(void);
char *ish_read_line(void);
char **ish_split_line(char *line);
int ish_launch(char **args);
int ish_cd(char **args);
int ish_exit(char **args);
int ish_help(char **args);
int ish_execute(char **args);

int main(int argc, char **argv) {

    ish_loop();

    return EXIT_SUCCESS;
}

void ish_loop(void) {
    int state = 1;
    do {
        printf("> ");
        char *line = ish_read_line();
        if (line == NULL) {
            state = 0;
        } else {
            char **args = ish_split_line(line);
            if (args[0] != NULL) {
                state = ish_execute(args);
            }
            free(line);
            free(args);   
        }
        
    } while (state != 0);
    
}

char *ish_read_line(void) {
    size_t capacity = 16;
    size_t length = 0;

    char *line = malloc(capacity);
    if (line == NULL) { 
        return NULL; // It could be that malloc cannot find a place on memory and returns null, you need to handle the case properly.
    }

    int ch; // chars in C are simply numbers, including "EOF" & "\n". Therefore we can represent char with an int data type.
    while ((ch = getchar()) != EOF && ch != '\n') // Use single quotes '\n' not "\n". Single quotes is one char (a number), double quotes is a string (a pointer to memory), so comparing ch with "\n" won't work.
    // // getchar() reads one char from the keyboard (stdin), not from a string. It waits until you press Enter, then gives the chars one by one.
    {
        if (length + 1 >= capacity) { // length + 1 because C marks where the string ends with "\0", so we need to keep a spot to add it ourselevs later.
            capacity *= 2;
            char *grown = realloc(line, capacity); // Why save it in grown? For the same reason above, if it returned null:
            if (grown == NULL) {
                free(line);
                return NULL; //  then we'll lose line pointer and becomes null "memory leak". It's better to have another varialbe, and after checking everthing is okay we can do line = grown
            }
            line = grown;
        }
        line[length] = (char)ch; // line[length++] = (char)ch
        length++; // The above comment line is the same as what I have, C gets the value of length first and use it as index, and then it increments.
    }

    if (ch == EOF && length == 0) { // If nothing was entered from stdio, then free line and return null.
        free(line);
        return NULL;
    }

    line[length] = '\0'; // Last thing in a string is \0, it denots the end of a string in C.
    return line;
}

char **ish_split_line(char *line) {
    size_t capacity = 16;
    size_t length = 0;

    char **tokens = malloc(capacity * sizeof(char*));
    if (tokens == NULL) { 
        return NULL;
    }
    char d[] = " ";
    char *token = strtok(line, d);

    do {
        if (length + 1 >= capacity) {
            capacity *= 2;
            char **grown_tokens = realloc(tokens, capacity * sizeof(char*));
            if (grown_tokens == NULL) {
                free(tokens);
                return NULL;
            }
            tokens = grown_tokens;
        }
        tokens[length] = token;
        length++;
        token = strtok(NULL, d);
    } while (token != NULL);
    tokens[length] = NULL;

    return tokens;
}

int ish_launch(char **args) {
    pid_t rc = fork();

    if (rc < 0) { // Fork Failed
        fprintf(stderr, "Fork failed\n");
        exit(1);
    } else if (rc == 0) { // This is the child
        if (execvp(args[0], args) == -1) {
            perror("ish");
        }
        exit(EXIT_FAILURE);
    } else { // This is the parent
        int status;
        do {
            waitpid(rc, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

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

int ish_execute(char **args) {
    if (strcmp(args[0], "cd") == 0) return ish_cd(args);
    else if (strcmp(args[0], "exit") == 0) return ish_exit(args);
    else if (strcmp(args[0], "help") == 0) return ish_help(args);
    else ish_launch(args);
    return 1;
}