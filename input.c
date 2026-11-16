#include "ish.h"

char *ish_read_line(FILE *stream) {
    size_t capacity = 16;
    size_t length = 0;

    char *line = malloc(capacity);
    if (line == NULL) { 
        return NULL; // It could be that malloc cannot find a place on memory and returns null, you need to handle the case properly.
    }

    int ch; // chars in C are simply numbers, including "EOF" & "\n". Therefore we can represent char with an int data type.
    while ((ch = fgetc(stream)) != EOF && ch != '\n') // Use single quotes '\n' not "\n". Single quotes is one char (a number), double quotes is a string (a pointer to memory), so comparing ch with "\n" won't work.
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

// Splits a line into words, handling quotes the way bash does:
//   'single quotes'  everything inside is literal
//   "double quotes"  backslash only escapes $, `, " and backslash
//   \x (unquoted)    x is taken literally, e.g. a\ b is one word
//   # at the start of a word begins a comment
// Works in place: the returned tokens point into line.
char **ish_split_line(char *line) {
    size_t capacity = 16;
    size_t length = 0;

    char **tokens = malloc(capacity * sizeof(char*));
    if (tokens == NULL) {
        return NULL;
    }

    char *r = line; // read position
    char *w = line; // write position (never ahead of r)
    while (1) {
        while (*r == ' ' || *r == '\t') r++;
        if (*r == '\0' || *r == '#') break;

        char *start = w;
        char quote = 0;
        while (*r) {
            char c = *r;
            if (quote == '\'') {
                if (c == '\'') quote = 0;
                else *w++ = c;
                r++;
            } else if (quote == '"') {
                if (c == '"') {
                    quote = 0;
                    r++;
                } else if (c == '\\' && r[1] && strchr("$`\"\\", r[1])) {
                    *w++ = r[1];
                    r += 2;
                } else {
                    *w++ = c;
                    r++;
                }
            } else {
                if (c == ' ' || c == '\t') break;
                if (c == '\'' || c == '"') {
                    quote = c;
                    r++;
                } else if (c == '\\' && r[1]) {
                    *w++ = r[1];
                    r += 2;
                } else {
                    *w++ = c;
                    r++;
                }
            }
        }

        int at_end = (*r == '\0');
        if (!at_end) r++;
        *w++ = '\0';

        if (length + 1 >= capacity) {
            capacity *= 2;
            char **grown_tokens = realloc(tokens, capacity * sizeof(char*));
            if (grown_tokens == NULL) {
                free(tokens);
                return NULL;
            }
            tokens = grown_tokens;
        }
        tokens[length++] = start;
        if (at_end) break;
    }
    tokens[length] = NULL;

    return tokens;
}
