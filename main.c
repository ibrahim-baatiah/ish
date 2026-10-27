#include <stdio.h>
#include <stdlib.h>

void ish_loop(void);
char *ish_read_line(void);

int main(int argc, char **argv) {

    ish_loop();

    return EXIT_SUCCESS;
}

void ish_loop(void) {
    printf(">");
    return;
}

char *ish_read_line(void) {
    char ch;
    char line[150];
    int i = 0;
    do {
        ch = getchar();
        line[i] = ch;
        i++;
    } while (i < 149 && ch != '\n');
    {
        line[i] = '\0';
        printf("Entered characters are %s", line); 
        return line; 
    }
}