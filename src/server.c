#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "protocol.h"
#include "ipc_channel.h"
#include "service_handler.h"

/* =========================================================================
 * MODULE 3 (Dev 3 làm chủ): src/server.c
 * -------------------------------------------------------------------------
 * - Hàm main() khởi chạy Server Daemon.
 * - Gọi Dev 1 (ipc_init) để mở kênh lắng nghe.
 * - Vòng lặp nhận yêu cầu từ các client (mtype = SERVER_MSG_TYPE).
 * - Sử dụng fork() xử lý đa tiến trình song song, gọi Dev 2 (handle_service).
 * - Gửi phản hồi định tuyến về đúng Client thông qua mtype = client_pid.
 * - Tự động thu hồi tiến trình con (tránh Zombie Process).
 * ========================================================================= */

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("=============================================\n");
    printf("           IPC SERVER (Dev 3 Main)           \n");
    printf("=============================================\n");

    /* Bound concurrent workers; reap completed children before each request. */
    unsigned workers = 0;
    enum { MAX_WORKERS = 32 };

    /* 1. Khởi tạo kênh IPC (Dev 1) */
    int qid = ipc_init(1);
    if (qid < 0) {
        fprintf(stderr, "[Server Error] Khong the khoi tao kenh IPC. Thoat.\n");
        return EXIT_FAILURE;
    }

    printf("[Server] Dang lang nghe yeu cau tren IPC Queue (qid=%d)...\n", qid);
    printf("[Server] Nhan Ctrl+C de dung Server va tu dong giai phong RAM.\n\n");

    msg_packet_t req;

    /* 2. Vòng lặp nhận và xử lý yêu cầu */
    while (1) {
        memset(&req, 0, sizeof(req));

        /* Nhận yêu cầu gửi tới server (mtype = SERVER_MSG_TYPE = 1) */
        if (ipc_recv(qid, &req, SERVER_MSG_TYPE) < 0) {
            /* Ngắt vòng lặp nếu có tín hiệu dừng hoặc lỗi hàng đợi */
            if (errno == EBADMSG) continue;
            break;
        }

        if (req.client_pid <= 1) continue;

        printf("[Server] <- Nhan request tu Client [%d]: \"%s\"\n", req.client_pid, req.payload);

        while (waitpid(-1, NULL, WNOHANG) > 0) {
            if (workers) --workers;
        }
        if (workers >= MAX_WORKERS) {
            pid_t done;
            do { done = waitpid(-1, NULL, 0); } while (done < 0 && errno == EINTR);
            if (done > 0) --workers;
        }

        /* 3. Tạo tiến trình con xử lý đa nhiệm (Concurrency qua fork) */
        pid_t pid = fork();
        if (pid < 0) {
            perror("[Server Warning] fork that bai, chuyen sang xu ly tuan tu");

            /* Fallback xử lý tuần tự nếu cạn tài nguyên hệ thống */
            msg_packet_t res;
            memset(&res, 0, sizeof(res));
            res.mtype = req.client_pid;
            res.client_pid = req.client_pid;
            res.status_code = handle_service(req.payload, res.payload);
            ipc_send(qid, &res);
        } else if (pid == 0) {
            /* Tiến trình con (Worker Child Process) */
            msg_packet_t res;
            memset(&res, 0, sizeof(res));
            res.mtype = req.client_pid;         /* Gán mtype = client_pid để định tuyến đúng client */
            res.client_pid = req.client_pid;

            /* Gọi module xử lý dịch vụ của Dev 2 */
            res.status_code = handle_service(req.payload, res.payload);

            /* Gửi kết quả về cho client qua hàm của Dev 1 */
            if (ipc_send(qid, &res) < 0) {
                fprintf(stderr, "[Worker %d] Gui phan hoi cho Client [%d] that bai.\n", (int)getpid(), req.client_pid);
            } else {
                printf("[Worker %d] -> Da phan hoi Client [%d] (status=%d): \"%s\"\n",
                       (int)getpid(), req.client_pid, res.status_code, res.payload);
            }

            _exit(EXIT_SUCCESS);
        }
        if (pid > 0) ++workers;
        /* Tiến trình cha tiếp tục vòng lặp để nhận request tiếp theo */
    }

    ipc_destroy(qid);
    printf("[Server] Server ket thuc.\n");
    return EXIT_SUCCESS;
}
