#include "service_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* =========================================================================
 * MODULE 2 (Dev 2 làm chủ): src/service_handler.c
 * -------------------------------------------------------------------------
 * Xử lý nghiệp vụ chuỗi lệnh:
 * - CALC    : Tính toán số học (+, -, *, /, %) và bắt lỗi chia cho 0.
 * - UPPER   : Chuyển toàn bộ chuỗi sang in hoa.
 * - REVERSE : Đảo ngược chuỗi ký tự.
 * - STATS   : Thống kê độ dài chuỗi và số lượng từ.
 * ========================================================================= */

/* Xử lý lệnh CALC <so1> <phep_toan> <so2> */
static int handle_calc(const char *args, char *out) {
    if (!args || *args == '\0') {
        snprintf(out, MAX_PAYLOAD_SIZE, "Loi: CALC yeu cau tham so: CALC <so1> <phep_toan> <so2>");
        return STATUS_ERROR_SYNTAX;
    }

    double a, b;
    char op;
    if (sscanf(args, "%lf %c %lf", &a, &op, &b) != 3) {
        snprintf(out, MAX_PAYLOAD_SIZE, "Loi cu phap: CALC <so1> <phep_toan> <so2> (VD: CALC 12 + 30)");
        return STATUS_ERROR_SYNTAX;
    }

    double result = 0.0;
    switch (op) {
        case '+':
            result = a + b;
            break;
        case '-':
            result = a - b;
            break;
        case '*':
        case 'x':
        case 'X':
            result = a * b;
            break;
        case '/':
            if (b == 0.0) {
                snprintf(out, MAX_PAYLOAD_SIZE, "Loi toan hoc: Khong the chia cho 0");
                return STATUS_ERROR_CALC;
            }
            result = a / b;
            break;
        case '%':
            if ((long)b == 0) {
                snprintf(out, MAX_PAYLOAD_SIZE, "Loi toan hoc: Phep chia lay du cho 0");
                return STATUS_ERROR_CALC;
            }
            snprintf(out, MAX_PAYLOAD_SIZE, "%ld", (long)a % (long)b);
            return STATUS_OK;
        default:
            snprintf(out, MAX_PAYLOAD_SIZE, "Loi: Toan tu '%c' khong hop le (+, -, *, /, %%)", op);
            return STATUS_ERROR_SYNTAX;
    }

    snprintf(out, MAX_PAYLOAD_SIZE, "%g", result);
    return STATUS_OK;
}

/* Xử lý lệnh UPPER <chuỗi> */
static int handle_upper(const char *args, char *out) {
    if (!args) {
        out[0] = '\0';
        return STATUS_OK;
    }

    size_t i = 0;
    for (; args[i] != '\0' && i < MAX_PAYLOAD_SIZE - 1; ++i) {
        out[i] = (char)toupper((unsigned char)args[i]);
    }
    out[i] = '\0';
    return STATUS_OK;
}

/* Xử lý lệnh REVERSE <chuỗi> */
static int handle_reverse(const char *args, char *out) {
    if (!args) {
        out[0] = '\0';
        return STATUS_OK;
    }

    size_t len = strlen(args);
    if (len >= MAX_PAYLOAD_SIZE) {
        len = MAX_PAYLOAD_SIZE - 1;
    }

    for (size_t i = 0; i < len; ++i) {
        out[i] = args[len - 1 - i];
    }
    out[len] = '\0';
    return STATUS_OK;
}

/* Xử lý lệnh STATS <chuỗi> */
static int handle_stats(const char *args, char *out) {
    if (!args) {
        snprintf(out, MAX_PAYLOAD_SIZE, "length=0 words=0");
        return STATUS_OK;
    }

    size_t len = strlen(args);
    size_t words = 0;
    int in_word = 0;

    for (size_t i = 0; args[i] != '\0'; ++i) {
        if (isspace((unsigned char)args[i])) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            words++;
        }
    }

    snprintf(out, MAX_PAYLOAD_SIZE, "length=%zu words=%zu", len, words);
    return STATUS_OK;
}

int handle_service(const char *in, char *out) {
    if (!in || !out) {
        return STATUS_ERROR_SYNTAX;
    }

    /* Bỏ qua khoảng trắng đầu dòng */
    while (*in && isspace((unsigned char)*in)) {
        in++;
    }

    if (*in == '\0') {
        snprintf(out, MAX_PAYLOAD_SIZE, "Yeu cau rong");
        return STATUS_ERROR_SYNTAX;
    }

    /* Tách tên lệnh (command) */
    char cmd[32];
    int offset = 0;
    if (sscanf(in, "%31s%n", cmd, &offset) != 1) {
        snprintf(out, MAX_PAYLOAD_SIZE, "Loi dinh dang yeu cau");
        return STATUS_ERROR_SYNTAX;
    }

    /* Tham số sau tên lệnh */
    const char *args = in + offset;
    while (*args && isspace((unsigned char)*args)) {
        args++;
    }

    if (strcasecmp(cmd, "CALC") == 0) {
        return handle_calc(args, out);
    } else if (strcasecmp(cmd, "UPPER") == 0) {
        return handle_upper(args, out);
    } else if (strcasecmp(cmd, "REVERSE") == 0) {
        return handle_reverse(args, out);
    } else if (strcasecmp(cmd, "STATS") == 0) {
        return handle_stats(args, out);
    } else {
        snprintf(out, MAX_PAYLOAD_SIZE, "Lenh khong hop le: '%s'. Ho tro: CALC, UPPER, REVERSE, STATS", cmd);
        return STATUS_ERROR_UNKNOWN;
    }
}
