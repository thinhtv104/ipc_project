#include "ipc_channel.h"
#include <stdio.h>
#include <stdlib.h>

/* =========================================================================
 * Dev 1 code: src/ipc_channel.c
 * -------------------------------------------------------------------------
 * Nhiệm vụ:
 * 1. Viết các hàm wrapper IPC (POSIX Message Queue hoặc System V).
 * 2. Cài đặt các hàm:
 *    - ipc_init()
 *    - ipc_send()
 *    - ipc_recv()
 *    - ipc_destroy()
 * 3. Bắt tín hiệu SIGINT để dọn sạch IPC trong RAM.
 * ========================================================================= */

int ipc_init(int is_server) {
    (void)is_server;
    /* TODO: Dev 1 code here */
    printf("[Dev 1 - TODO] ipc_init chua duoc cai dat.\n");
    return 0;
}

int ipc_send(int qid, const msg_packet_t *msg) {
    (void)qid;
    (void)msg;
    /* TODO: Dev 1 code here */
    printf("[Dev 1 - TODO] ipc_send chua duoc cai dat.\n");
    return 0;
}

int ipc_recv(int qid, msg_packet_t *msg, long target_type) {
    (void)qid;
    (void)msg;
    (void)target_type;
    /* TODO: Dev 1 code here */
    printf("[Dev 1 - TODO] ipc_recv chua duoc cai dat.\n");
    return 0;
}

void ipc_destroy(int qid) {
    (void)qid;
    /* TODO: Dev 1 code here */
    printf("[Dev 1 - TODO] ipc_destroy chua duoc cai dat.\n");
}
