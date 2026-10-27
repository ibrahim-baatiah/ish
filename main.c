#include <stdio.h>
#include <stdlib.h>

void ish_loop(void);
char *ish_read_line(void);

int main(int argc, char **argv) {

    ish_loop();

    return EXIT_SUCCESS;
}

void ish_loop(void) {
    // char *line = ish_read_line();
    // printf("You entered: %s\n", line);
    // free (line);
    // return;

    int state = 1;
    do {
        printf("> ");
        char *line = ish_read_line();
        if (line == NULL) {
            state = 0;
        } else {
            printf("You entered: %s\n", line);
            free(line);    
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
    while ((ch = getchar()) != EOF && ch != '\n')
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