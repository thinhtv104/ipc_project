#include "service_handler.h"
#include <stdio.h>
#include <string.h>

/* =========================================================================
 * Dev 2 code: src/service_handler.c
 * -------------------------------------------------------------------------
 * Nhiệm vụ:
 * 1. Viết bộ parser chuỗi lệnh (Tokenize chuỗi từ client).
 * 2. Cài đặt logic:
 *    - CALC   : tính toán số học (+, -, *, /, %)
 *    - UPPER  : chuyển chuỗi thành in hoa
 *    - REVERSE: đảo ngược chuỗi
 *    - STATS  : thống kê chuỗi (độ dài, số từ, v.v.)
 * 3. Hàm chính:
 *    int handle_service(const char *in, char *out);
 * ========================================================================= */

int handle_service(const char *in, char *out) {
    (void)in;
    (void)out;
    /* TODO: Dev 2 code here */
    printf("[Dev 2 - TODO] handle_service chua duoc cai dat.\n");
    if (out) {
        snprintf(out, MAX_PAYLOAD_SIZE, "Chua duoc cai dat");
    }
    return STATUS_OK;
}
