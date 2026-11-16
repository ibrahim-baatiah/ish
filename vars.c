#include "ish.h"

typedef struct {
    char *name;
    char *value;
} Variable;

static Variable *vars = NULL;
size_t capacity = 16;
size_t length = 0;

void ish_vars_init(size_t capacity) {
    vars = malloc(capacity * sizeof(Variable));
}

int ish_grow_vars(size_t cap) {
    capacity = cap * sizeof(Variable);
    Variable *grown_vars = realloc(vars, capacity);
    if (grown_vars == NULL) {
        free(vars);
        return -1;
    }
    vars = grown_vars;
    return 0;
}

int ish_is_assignment(char *token, char *eq) {
    if (vars == NULL) ish_vars_init(16);    
    *eq = '\0';
    char *after_eq = eq + 1;
    ish_set_var(token, after_eq);
    return 1;
}

int ish_set_var(char *name, char *value) {
    for  (int i = 0; i < length; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            free(vars[i].value);
            vars[i].value = strdup(value);
            return 0;
        } else {
            if (length + 1 >= capacity) ish_grow_vars(capacity*2);
        } 
    }
    vars[length].name = strdup(name);
    vars[length].value = strdup(value);
    length++;
    return 0;
}

char *ish_get_var(char *name) {
    for (int i = 0; i < length; i++) {
        if (strcmp(vars[i].name, name) == 0) return vars[i].value;
    }
    if (getenv(name) != NULL) return getenv(name);
    return NULL;
}

char *ish_expand_token(char *token) {
    if (token[0] == '$') {
        char *val = ish_get_var(token + 1);
        return val != NULL ? val : "\n";
    }
    return token;
}