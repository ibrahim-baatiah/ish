#include "ish.h"
#include <pwd.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define ISH_VERSION "0.1"
#define ISH_RELEASE "0.1.0"
// Used when PS1 isn't set by the user or inherited from the environment
#define ISH_DEFAULT_PS1 "\\u@\\h:\\w\\$ "

static int command_number = 1;

// Small growable string used to build the prompt
typedef struct {
    char *data;
    size_t len;
    size_t cap;
} Buf;

static void buf_putn(Buf *b, const char *s, size_t n) {
    if (b->len + n + 1 > b->cap) {
        size_t cap = b->cap ? b->cap : 64;
        while (b->len + n + 1 > cap) cap *= 2;
        char *grown = realloc(b->data, cap);
        if (grown == NULL) return;
        b->data = grown;
        b->cap = cap;
    }
    memcpy(b->data + b->len, s, n);
    b->len += n;
    b->data[b->len] = '\0';
}

static void buf_puts(Buf *b, const char *s) { buf_putn(b, s, strlen(s)); }
static void buf_putc(Buf *b, char c) { buf_putn(b, &c, 1); }

static const char *user_name(void) {
    struct passwd *pw = getpwuid(geteuid());
    if (pw != NULL && pw->pw_name != NULL) return pw->pw_name;
    const char *user = getenv("USER");
    return user ? user : "?";
}

// \h = hostname up to the first '.', \H = full hostname
static void put_host(Buf *b, int full) {
    char host[256];
    if (gethostname(host, sizeof host) != 0) {
        buf_puts(b, "?");
        return;
    }
    host[sizeof host - 1] = '\0';
    if (!full) {
        char *dot = strchr(host, '.');
        if (dot) *dot = '\0';
    }
    buf_puts(b, host);
}

// \w = full path with $HOME shown as ~, \W = last part only
static void put_cwd(Buf *b, int basename_only) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof cwd) == NULL) {
        buf_puts(b, "?");
        return;
    }
    const char *home = getenv("HOME");
    size_t hl = home ? strlen(home) : 0;
    int under_home = hl > 1 && strncmp(cwd, home, hl) == 0 &&
                     (cwd[hl] == '\0' || cwd[hl] == '/');

    if (basename_only) {
        if (under_home && cwd[hl] == '\0') buf_puts(b, "~");
        else if (strcmp(cwd, "/") == 0) buf_puts(b, "/");
        else buf_puts(b, strrchr(cwd, '/') + 1);
    } else if (under_home) {
        buf_putc(b, '~');
        buf_puts(b, cwd + hl);
    } else {
        buf_puts(b, cwd);
    }
}

static void put_time(Buf *b, const char *fmt) {
    char out[256];
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    size_t n = tm ? strftime(out, sizeof out, fmt, tm) : 0;
    buf_putn(b, out, n);
}

// \l = basename of the terminal device
static void put_tty(Buf *b) {
    char *tty = ttyname(STDIN_FILENO);
    if (tty == NULL) {
        buf_puts(b, "tty");
        return;
    }
    char *slash = strrchr(tty, '/');
    buf_puts(b, slash ? slash + 1 : tty);
}

// $NAME or ${NAME} inside the prompt. Returns 1 if it consumed a variable.
static int put_var(Buf *b, const char **pp) {
    const char *p = *pp + 1;
    int braced = (*p == '{');
    if (braced) p++;
    if (!(isalpha((unsigned char)*p) || *p == '_')) return 0;

    const char *start = p;
    while (isalnum((unsigned char)*p) || *p == '_') p++;
    if (braced && *p != '}') return 0;

    char name[128];
    size_t n = p - start;
    if (n >= sizeof name) n = sizeof name - 1;
    memcpy(name, start, n);
    name[n] = '\0';

    const char *val = ish_get_var(name);
    if (val) buf_puts(b, val);

    // Leave *pp on the last consumed char; the caller's loop does p++
    *pp = braced ? p : p - 1;
    return 1;
}

void ish_prompt_init(void) {
    if (ish_get_var("PS1") == NULL) ish_set_var("PS1", ISH_DEFAULT_PS1);
}

void ish_prompt_next_command(void) {
    command_number++;
}

// Expands PS1 the same way bash does. Caller must free the result.
char *ish_render_prompt(void) {
    const char *ps1 = ish_get_var("PS1");
    if (ps1 == NULL) ps1 = "";

    Buf b = {0};
    for (const char *p = ps1; *p; p++) {
        if (*p == '$' && put_var(&b, &p)) continue;
        if (*p != '\\' || p[1] == '\0') {
            buf_putc(&b, *p);
            continue;
        }

        p++;
        switch (*p) {
        case 'a': buf_putc(&b, '\a'); break;
        case 'd': put_time(&b, "%a %b %d"); break;
        case 'D': {
            const char *end = (p[1] == '{') ? strchr(p + 2, '}') : NULL;
            if (end == NULL) {
                buf_puts(&b, "\\D");
                break;
            }
            char fmt[128];
            size_t n = end - (p + 2);
            if (n >= sizeof fmt) n = sizeof fmt - 1;
            memcpy(fmt, p + 2, n);
            fmt[n] = '\0';
            put_time(&b, n ? fmt : "%X");
            p = end;
            break;
        }
        case 'e': buf_putc(&b, '\033'); break;
        case 'h': put_host(&b, 0); break;
        case 'H': put_host(&b, 1); break;
        case 'j': buf_putc(&b, '0'); break; // no job control yet
        case 'l': put_tty(&b); break;
        case 'n': buf_putc(&b, '\n'); break;
        case 'r': buf_putc(&b, '\r'); break;
        case 's': buf_puts(&b, "ish"); break;
        case 't': put_time(&b, "%H:%M:%S"); break;
        case 'T': put_time(&b, "%I:%M:%S"); break;
        case '@': put_time(&b, "%I:%M %p"); break;
        case 'A': put_time(&b, "%H:%M"); break;
        case 'u': buf_puts(&b, user_name()); break;
        case 'v': buf_puts(&b, ISH_VERSION); break;
        case 'V': buf_puts(&b, ISH_RELEASE); break;
        case 'w': put_cwd(&b, 0); break;
        case 'W': put_cwd(&b, 1); break;
        case '!':
        case '#': {
            char num[32];
            snprintf(num, sizeof num, "%d", command_number);
            buf_puts(&b, num);
            break;
        }
        case '$': buf_putc(&b, geteuid() == 0 ? '#' : '$'); break;
        case '\\': buf_putc(&b, '\\'); break;
        case '[':
        case ']': break; // readline markers; nothing to do without readline
        default:
            if (*p >= '0' && *p <= '7') { // \nnn octal
                int value = 0, k = 0;
                while (k < 3 && p[k] >= '0' && p[k] <= '7') {
                    value = value * 8 + (p[k] - '0');
                    k++;
                }
                buf_putc(&b, (char)value);
                p += k - 1;
            } else { // unknown escape: print it as-is, like bash
                buf_putc(&b, '\\');
                buf_putc(&b, *p);
            }
        }
    }
    return b.data ? b.data : strdup("");
}
