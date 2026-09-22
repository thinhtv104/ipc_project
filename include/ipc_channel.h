#ifndef IPC_CHANNEL_H
#define IPC_CHANNEL_H

#include "protocol.h"

/* =========================================================================
 * Module 1 (Dev 1 làm chủ)
 * Nhiệm vụ:
 * - Viết các hàm wrapper IPC (POSIX Message Queue hoặc System V).
 * - ipc_init(), ipc_send(), ipc_recv(), ipc_destroy().
 * - Bắt tín hiệu SIGINT để dọn sạch IPC trong RAM.
 * ========================================================================= */

int ipc_init(int is_server);
int ipc_send(int qid, const msg_packet_t *msg);
int ipc_recv(int qid, msg_packet_t *msg, long target_type);
void ipc_destroy(int qid);

#endif /* IPC_CHANNEL_H */
