#include <stdio.h>
#include <stdlib.h>

void ish_loop(void);

int main(int argc, char **argv) {

    ish_loop();

    return EXIT_SUCCESS;
}

void ish_loop(void) {
    printf(">");
    return;
}