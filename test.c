#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    int x = 10;
    int *ptr = &x;

    printf("Value of x: %d\n", x);
    printf("Value of ptr: %d\n", ptr);
    printf("Value pointed to by ptr: %d\n", *ptr);

    char *str[] = {"Hello", "World", "!"};
    char **words = str;
    
    printf("-- Using Words -- First word: %s\n", words[0]);
    printf("-- Using Words -- First letter, second word: %c\n", words[1][0]);

    printf("-- Using str -- First word: %s\n", str[0]);
    printf("-- Using str -- First letter, second word: %c\n", str[1][0]);
    
    return EXIT_SUCCESS;
}

