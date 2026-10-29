#include <stdio.h>
#include <string.h>
#include "service_handler.h"

int main(void) {
    const struct { const char *in; int status; const char *out; } cases[] = {
        {"CALC 12 + 30", STATUS_OK, "42"},
        {"calc -2.5 * 4", STATUS_OK, "-10"},
        {"CALC 10 % 3", STATUS_OK, "1"},
        {"CALC -10 % 3", STATUS_OK, "-1"},
        {"CALC 1 / 0", STATUS_ERROR_CALC, NULL},
        {"CALC 1 % 0", STATUS_ERROR_CALC, NULL},
        {"CALC 1.5 % 2", STATUS_ERROR_CALC, NULL},
        {"CALC 1 + 2 garbage", STATUS_ERROR_SYNTAX, NULL},
        {"CALC nan + 2", STATUS_ERROR_SYNTAX, NULL},
        {"CALC 1e999 + 2", STATUS_ERROR_SYNTAX, NULL},
        {"CALC 1e308 * 10", STATUS_ERROR_CALC, NULL},
        {"CALC 1 +", STATUS_ERROR_SYNTAX, NULL},
        {"UPPER hello", STATUS_OK, "HELLO"},
        {"REVERSE abc", STATUS_OK, "cba"},
        {"STATS one two", STATUS_OK, "length=7 words=2"},
        {"STATS", STATUS_OK, "length=0 words=0"},
        {"", STATUS_ERROR_SYNTAX, NULL},
        {"unknown", STATUS_ERROR_UNKNOWN, NULL},
        {"HELP", STATUS_OK, NULL}
    };
    unsigned failed = 0;
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        char out[MAX_PAYLOAD_SIZE] = {0};
        int status = handle_service(cases[i].in, out);
        if (status != cases[i].status ||
            (cases[i].out && strcmp(out, cases[i].out))) {
            fprintf(stderr, "FAIL: %s => [%d] %s\n", cases[i].in, status, out);
            ++failed;
        }
    }
    printf("Service tests: %u failed\n", failed);
    return failed ? 1 : 0;
}
