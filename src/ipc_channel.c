#include "ipc_channel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/* =========================================================================
 * MODULE 1 (Dev 1 làm chủ): src/ipc_channel.c
 * -------------------------------------------------------------------------
 * Cài đặt các hàm wrapper IPC (System V Message Queue) và quản lý tài nguyên:
 * - ipc_init(): Khởi tạo hàng đợi, bắt SIGINT/SIGTERM để dọn RAM khi tắt.
 * - ipc_send(): Gửi thông điệp (bỏ qua kích thước mtype theo chuẩn System V).
 * - ipc_recv(): Nhận thông điệp theo mtype (1 cho server, client_pid cho client).
 * - ipc_destroy(): Xóa hàng đợi khỏi kernel RAM.
 * ========================================================================= */

static int g_active_qid = -1;
static pid_t g_owner_pid = -1;

/* Bắt tín hiệu SIGINT (Ctrl+C) và SIGTERM để dọn sạch IPC trong RAM */
static void sigint_cleanup_handler(int signum) {
    (void)signum;
    if (g_active_qid >= 0 && getpid() == g_owner_pid)
        msgctl(g_active_qid, IPC_RMID, NULL);
    _exit(0);
}

int ipc_init(int is_server) {
    key_t key = ftok(IPC_PROJECT_KEY_PATH, IPC_PROJECT_PROJ_ID);
    if (key == (key_t)-1) {
        perror("ftok");
        return -1;
    }

    int qid;
    if (is_server) {
        /* Server tạo mới hoặc mở hàng đợi với quyền 0666 */
        qid = msgget(key, IPC_CREAT | IPC_EXCL | 0600);
        if (qid < 0) {
            perror("[IPC Error] Server msgget that bai");
            return -1;
        }

        g_active_qid = qid;
        g_owner_pid = getpid();

        /* Đăng ký signal handler để tự động dọn dẹp hàng đợi khi tắt server */
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sigint_cleanup_handler;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);

        printf("[IPC] Server da khoi tao hang doi thanh cong (key=0x%x, qid=%d)\n", (unsigned int)key, qid);
    } else {
        /* Client kết nối tới hàng đợi sẵn có do Server tạo */
        qid = msgget(key, 0600);
        if (qid < 0) {
            perror("[IPC Error] Client khong the ket noi den hang doi (Server chua khoi dong?)");
            return -1;
        }
    }

    return qid;
}

int ipc_send(int qid, const msg_packet_t *msg) {
    if (qid < 0 || !msg || msg->mtype <= 0) {
        errno = EINVAL;
        return -1;
    }

    /* Kích thước dữ liệu gửi đi không tính trường mtype */
    size_t payload_size = sizeof(msg_packet_t) - sizeof(long);

    int rc;
    do { rc = msgsnd(qid, msg, payload_size, IPC_NOWAIT); } while (rc < 0 && errno == EINTR);
    if (rc == -1) {
        perror("[IPC Error] msgsnd gui thong diep that bai");
        return -1;
    }
    return 0;
}

int ipc_recv(int qid, msg_packet_t *msg, long target_type) {
    if (qid < 0 || !msg || target_type <= 0) {
        errno = EINVAL;
        return -1;
    }

    size_t payload_size = sizeof(msg_packet_t) - sizeof(long);

    /* Nhận đúng thông điệp có mtype == target_type */
    ssize_t received;
    do {
        received = msgrcv(qid, msg, payload_size, target_type, MSG_NOERROR);
    } while (received < 0 && errno == EINTR);
    if (received == -1) {
        if (errno != EINTR) {
            perror("[IPC Error] msgrcv nhan thong diep that bai");
        }
        return -1;
    }
    if ((size_t)received != payload_size ||
        !memchr(msg->payload, '\0', sizeof(msg->payload))) {
        errno = EBADMSG;
        return -1;
    }
    return 0;
}

void ipc_destroy(int qid) {
    if (qid >= 0) {
        if (msgctl(qid, IPC_RMID, NULL) == -1) {
            perror("[IPC Error] msgctl IPC_RMID that bai");
        } else {
            printf("[IPC] Da giai phong hang doi IPC (qid=%d) khoi RAM.\n", qid);
        }
    }
}
