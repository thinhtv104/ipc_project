#ifndef SERVICE_HANDLER_H
#define SERVICE_HANDLER_H

#include "protocol.h"

/* =========================================================================
 * Module 2 (Dev 2 làm chủ)
 * Nhiệm vụ:
 * - Viết bộ parser chuỗi lệnh (Tokenize chuỗi từ client).
 * - Cài đặt logic: CALC (tính toán), UPPER, REVERSE, STATS.
 * - Hàm chính: int handle_service(const char *in, char *out);
 * ========================================================================= */

int handle_service(const char *in, char *out);

#endif /* SERVICE_HANDLER_H */
