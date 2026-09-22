#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"
#include "ipc_channel.h"
#include "service_handler.h"

/* =========================================================================
 * Dev 3 code: src/server.c
 * -------------------------------------------------------------------------
 * Nhiệm vụ:
 * 1. Viết hàm main() của Server.
 * 2. Gọi hàm của Dev 1 (ipc_init) để tạo kênh lắng nghe.
 * 3. Vòng lặp nhận tin, xử lý đa tiến trình (fork() hoặc tuần tự),
 *    gọi hàm Dev 2 (handle_service) để xử lý, rồi gửi phản hồi (ipc_send).
 * ========================================================================= */

int main(void) {
    printf("=============================================\n");
    printf("           IPC SERVER (Dev 3 Main)           \n");
    printf("=============================================\n");

    /* TODO: Dev 3 code here */
    printf("[Dev 3 - TODO] Server main chua duoc cai dat day du.\n");

    return 0;
}
